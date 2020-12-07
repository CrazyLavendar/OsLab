#define _POSIX_C_SOURCE 1
#include <malloc.h>
#include <ucontext.h>
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#define MAXTHREADS 20
#define THREAD_STACKSIZE 32767

typedef struct thread_t
{
    int thr_id;
    int thr_usrpri;
    int thr_cpupri;
    int thr_pri;
    int thr_nice;

    int thr_totalcpu;
    ucontext_t thr_context;
    ucontext_t thr_succ_context;
    void *thr_stack;
    int thr_stacksize;
    struct thread_t *thr_next;
    struct thread_t *thr_prev;
} thread_t;

typedef struct mutex_t
{
    int val;
    thread_t *owner;
    thread_t *wait_q;
} mutex_t;

typedef struct condition_t
{
    thread_t *wait_q;
} condition_t;

thread_t thread_table[MAXTHREADS];
unsigned int whichqs = 0;
thread_t **qs, **tqs;

int ticks = 0;
int timeq = 0;
thread_t *ready_q = NULL, *current_thread = NULL;
void timertick_handler(int alrm);
void schedule(void);
void enqueue_thread(thread_t *t, thread_t *qs[]);
void thread_exit(void);

thread_t *thread_create(void (*thr_func)(void *), void *arg)
{
    int i;
    thread_t *thr;
    static int thrno = 1;
    struct itimerval t1;

    // Allocate thread structure.
    for (i = 0; i < MAXTHREADS; i++)
        if (thread_table[i].thr_id == 0) // Unused
            break;

    if (i >= MAXTHREADS)
        return NULL;

    thr = &thread_table[i];
    // Get the current execution context
    getcontext(&(thr->thr_context));
    getcontext(&(thr->thr_succ_context));

    /*thr->thr_succ_context.uc_link = 0;
         thr->thr_succ_context.uc_stack.ss_sp = malloc( THREAD_STACKSIZE );
         thr->thr_succ_context.uc_stack.ss_size = THREAD_STACKSIZE;
         thr->thr_succ_context.uc_stack.ss_flags = 0;  
	 makecontext( &thr->thr_succ_context, (void (*)())(&thread_exit), 0 );  */

    // Modify the context to a new stack
    thr->thr_context.uc_link = &thr->thr_succ_context;
    thr->thr_stack = thr->thr_context.uc_stack.ss_sp = malloc(THREAD_STACKSIZE);
    thr->thr_context.uc_stack.ss_size = THREAD_STACKSIZE;
    thr->thr_context.uc_stack.ss_flags = 0;

    thr->thr_succ_context.uc_link = 0;
    thr->thr_succ_context.uc_stack.ss_sp = thr->thr_context.uc_stack.ss_sp;
    thr->thr_succ_context.uc_stack.ss_size = THREAD_STACKSIZE;
    thr->thr_succ_context.uc_stack.ss_flags = 0;
    makecontext(&thr->thr_succ_context, (void (*)())(&thread_exit), 0);

    if (thr->thr_context.uc_stack.ss_sp == 0)
    {
        perror("malloc: Could not allocate stack");
        exit(1);
    }

    // Create the new context
    printf("Creating child fiber\n");
    makecontext(&thr->thr_context, (void (*)())thr_func, 1, arg);

    // Initialize other fields of the thread structure
    thr->thr_id = thrno++;
    thr->thr_usrpri = 20;
    thr->thr_cpupri = 0;
    thr->thr_totalcpu = 0;
    thr->thr_stacksize = THREAD_STACKSIZE;
    thr->thr_nice = 20;
    thr->thr_pri = 20;

    if (qs == NULL && tqs == NULL)
    {
        qs = (thread_t **)malloc(32 * sizeof(thread_t *));
        tqs = (thread_t **)malloc(32 * sizeof(thread_t *));

        // Create the main thread also
        for (i = 0; i < MAXTHREADS; i++)
            if (thread_table[i].thr_id == 0)
            {
                current_thread = &thread_table[i];
                current_thread->thr_id = thrno++;
                current_thread->thr_usrpri = 20;
                current_thread->thr_cpupri = 0;
                current_thread->thr_totalcpu = 0;
                current_thread->thr_nice = 20;
                current_thread->thr_pri = 20;
                current_thread->thr_next = NULL;
                current_thread->thr_prev = NULL;

                break;
            }
        // First thread so activate the timer tick
        signal(SIGPROF, timertick_handler);
        t1.it_interval.tv_usec = 50000;
        t1.it_interval.tv_sec = 0;
        t1.it_value.tv_usec = 0;
        t1.it_value.tv_sec = 2;
        setitimer(ITIMER_PROF, &t1, NULL);
    }

    enqueue_thread(thr, qs);

    return thr;
}

void thread_exit(void)
{

    printf("\nexit called\n");
    // Remove this thread from the ready_q
    // TO BE DONE :: Block the signals during adding to the ready_q;

    /*if (current_thread->thr_next != NULL)
		(current_thread->thr_next)->thr_prev = current_thread->thr_prev;
	if (current_thread->thr_prev != NULL)
		(current_thread->thr_prev)->thr_next = current_thread->thr_next;
	
	if (current_thread == ready_q)
		ready_q = current_thread->thr_next;*/
    free(current_thread->thr_stack);
    current_thread->thr_id = 0; // Free the slot

    current_thread = NULL;
    schedule();
    // DO NOT ENABLE SIGNALS HERE
}

void thread_yield(void)
{
    schedule();
}

void enqueue_thread(thread_t *t, thread_t *qs[])
{
    int id;
    thread_t *t1, *t2;

    // Find the queue id
    id = t->thr_pri / 4;
    //printf("*******in equeue thread id : %d\n",id);
    // Inser at proper position within the selected queue based priority
    // and modify whichqs
    if (qs[id] == NULL)
    {
        qs[id] = t;
        t->thr_prev = t->thr_next = NULL;
        whichqs = whichqs | (1 << id);
    }
    else
    {
        t1 = qs[id];
        // Insert at the beginning?
        if (t1->thr_pri > t->thr_pri)
        {
            t->thr_next = t1;
            t->thr_prev = NULL;
            t1->thr_prev = t;
            qs[id] = t;
        }
        else
        {
            while (t1->thr_next != NULL && (t1->thr_next)->thr_pri <= t->thr_pri)
            {
                t1 = t1->thr_next;
                //	printf("in while\n");
            }
            t->thr_next = t1->thr_next;
            t->thr_prev = t1;
            t1->thr_next = t;
            /*if (t->thr_next != NULL)
				(t->thr_next)->thr_prev = t;*/
        }
    }
}

void dequeue_thread(int id, thread_t *t, thread_t *qs[])
{
    if (t->thr_prev != NULL)
        (t->thr_prev)->thr_next = t->thr_next;
    else
        qs[id] = t->thr_next;

    if (t->thr_next != NULL)
        (t->thr_next)->thr_prev = t->thr_prev;

    t->thr_next = t->thr_prev = NULL;

    // Check if queue is empty?
    if (qs[id] == NULL)
        whichqs = whichqs & (~(1 << id));
}

void recompute_priority(void)
{
    int i;
    thread_t *t1, *t2;

    // For all the queues qs[], select one thread at a time,
    // compute its pririty, insert it into tqs[] queue
    //printf("recomputing priority called\n");

    for (i = 0; i < 32; i++)
    {
        while (qs[i] != NULL)
        {
            t1 = qs[i];
            //printf("recompute priority for %d\n",i);
            dequeue_thread(i, t1, qs);
            t1->thr_cpupri /= 2;
            t1->thr_pri = t1->thr_usrpri + (t1->thr_cpupri / 4) + (2 * t1->thr_nice);
            //printf("enqueue priority for %d\n",i);
            enqueue_thread(t1, tqs);
        }
    }
    for (i = 0; i < 32; i++)
    {
        qs[i] = tqs[i];
        tqs[i] = NULL;
    }
}

void schedule()
{
    int id;
    thread_t *t, *t1;
    ucontext_t dummy;
    //printf("whichqs : %d\n",whichqs);

    // Select the first thread in first non-empty queue
    // dequeue it and context switch
    for (id = 0; id < 32; id++)
        if (whichqs & (1 << id))
            break;

    // Check if non-empty queue is found?
    //printf("id in scheduler:%d\n",id);
    if (id < 32)
    {
        t = qs[id];
        dequeue_thread(id, t, qs);

        // If current thread is active then enqueue it.
        if (current_thread != NULL)
        {
            enqueue_thread(current_thread, qs);
            t1 = current_thread;
            current_thread = t;
            if (-1 == swapcontext(&t1->thr_context, &t->thr_context))
                perror("swapcontext error: ");
        }
        else
        {
            current_thread = t;
            if (-1 == swapcontext(&dummy, &t->thr_context))
                perror("swapcontext error: ");
        }
    }
    else
    {
        // Deadlock situtation is not addressed.
        // Assuming that at least one runnable thread must be available.
        if (current_thread != NULL)
            return;
        else
            exit(0);
    }
}

void timertick_handler(int alrm)
{
    thread_t *t1;
    //printf("tick handler called\n");
    ticks++;
    current_thread->thr_totalcpu += 10; // One timer tick = 10 Units
    current_thread->thr_cpupri += 10;   // One timer tick
    timeq += 10;
    //printf("timeq=%d\n",timeq);
    if (ticks % 25 == 0)
    {
        // recalculate priorities of the other threads
        recompute_priority();
    }
    if (timeq >= 50)
    {
        // Time quantum expired?
        schedule();
    }
}

void mutex_init(mutex_t *mut)
{
    mut->val = 1;
    mut->owner = NULL;
    mut->wait_q = NULL;
}

int mutex_lock(mutex_t *mut)
{
    sigset_t sigt;
    thread_t *t1;

    // Block timer
    sigemptyset(&sigt);
    sigaddset(&sigt, SIGPROF);
    sigprocmask(SIG_BLOCK, &sigt, NULL);

    mut->val--;
    if (mut->val < 0)
    {

        mut->val++;
        // Sleep, remove from ready_q
        /*if (current_thread->thr_prev != NULL)
			(current_thread->thr_prev)->thr_next = current_thread->thr_next;
		if (current_thread->thr_next != NULL)
			(current_thread->thr_next)->thr_prev = current_thread->thr_prev;

		if (current_thread->thr_prev == NULL)
			ready_q = current_thread->thr_next;*/

        // Add this to the waiting q
        if (mut->wait_q == NULL)
        {
            current_thread->thr_prev = NULL;
            current_thread->thr_next = NULL;
            mut->wait_q = current_thread;
        }
        else
        {
            t1 = mut->wait_q;
            while (t1->thr_next != NULL)
                t1 = t1->thr_next;
            t1->thr_next = current_thread;
            current_thread->thr_next = NULL;
            current_thread->thr_prev = t1;
        }

        current_thread = NULL;

        // Call scheduler
        schedule();
    }

    // Lock it
    mut->owner = current_thread;
    // Enable timer
    sigprocmask(SIG_UNBLOCK, &sigt, NULL);
    return 0;
}

int mutex_trylock(mutex_t *mut)
{
    sigset_t sigt;

    // Block timer
    sigemptyset(&sigt);
    sigaddset(&sigt, SIGPROF);
    sigprocmask(SIG_BLOCK, &sigt, NULL);

    if (mut->val <= 0)
    {
        sigprocmask(SIG_UNBLOCK, &sigt, NULL);
        return -1;
    }

    // Lock it
    mut->val--;
    mut->owner = current_thread;
    // Enable timer
    sigprocmask(SIG_UNBLOCK, &sigt, NULL);
    return 0;
}

int mutex_unlock(mutex_t *mut)
{
    sigset_t sigt;
    thread_t *t1;

    if (mut->owner != current_thread)
        return -1; // Error
    // Block timer
    sigemptyset(&sigt);
    sigaddset(&sigt, SIGPROF);
    sigprocmask(SIG_BLOCK, &sigt, NULL);

    mut->val++;
    if (mut->val == 1)
    {
        // wakeup the first waiting thread.
        t1 = mut->wait_q;
        mut->wait_q = t1->thr_next;
        mut->owner = NULL;
        // enqueue t1 to qs
        t1->thr_next = NULL;
        t1->thr_prev = NULL;
        enqueue_thread(t1, qs);
    }

    // Enable timer
    sigprocmask(SIG_UNBLOCK, &sigt, NULL);
    return 0;
}

void condition_init(condition_t *cond)
{
    cond->wait_q = NULL;
}

void condition_wait(condition_t *cond, mutex_t *mut)
{
    sigset_t sigt;
    thread_t *t1;

    // Block timer
    sigemptyset(&sigt);
    sigaddset(&sigt, SIGPROF);
    sigprocmask(SIG_BLOCK, &sigt, NULL);

    // Sleep, remove from ready_q
    /*if (current_thread->thr_prev != NULL)
		(current_thread->thr_prev)->thr_next = current_thread->thr_next;
	if (current_thread->thr_next != NULL)
		(current_thread->thr_next)->thr_prev = current_thread->thr_prev;

	if (current_thread->thr_prev == NULL)
		ready_q = current_thread->thr_next;*/

    // Add this to the waiting q
    if (cond->wait_q == NULL)
    {
        current_thread->thr_prev = NULL;
        current_thread->thr_next = NULL;
        cond->wait_q = current_thread;
    }
    else
    {
        t1 = cond->wait_q;
        while (t1->thr_next != NULL)
            t1 = t1->thr_next;
        t1->thr_next = current_thread;
        current_thread->thr_next = NULL;
        current_thread->thr_prev = t1;
    }

    current_thread = NULL;

    // Call scheduler
    schedule();
    sigprocmask(SIG_UNBLOCK, &sigt, NULL);
}

int condition_signal(condition_t *cond)
{
    sigset_t sigt;
    thread_t *t1;

    // Block timer
    sigemptyset(&sigt);
    sigaddset(&sigt, SIGPROF);
    sigprocmask(SIG_BLOCK, &sigt, NULL);

    // wakeup the first waiting thread.
    t1 = cond->wait_q;
    if (t1 != NULL)
    {
        cond->wait_q = t1->thr_next;
        // enqueue t1 to qs
        t1->thr_next = NULL;
        t1->thr_prev = NULL;
        enqueue_thread(t1, qs);
    }
    // Enable timer
    sigprocmask(SIG_UNBLOCK, &sigt, NULL);
    return 0;
}

int condition_broadcast(condition_t *cond)
{
    sigset_t sigt;
    thread_t *t1, *t2;

    // Block timer
    sigemptyset(&sigt);
    sigaddset(&sigt, SIGPROF);
    sigprocmask(SIG_BLOCK, &sigt, NULL);

    // wakeup all waiting threads.
    t1 = cond->wait_q;
    cond->wait_q = NULL;
    while (t1 != NULL)
    {
        t2 = t1->thr_next;

        // enqueue t1 to qs
        t1->thr_next = NULL;
        t1->thr_prev = NULL;
        enqueue_thread(t1, qs);

        t1 = t2;
    }

    // Enable timer
    sigprocmask(SIG_UNBLOCK, &sigt, NULL);
    return 0;
}

// The child thread will execute this function
void threadFunction(void *arg)
{
    int i, thrno, j;
    thrno = *(int *)arg;
    for (j = 1; j < 100 * thrno; j++)
    {
        printf("This is child thread : %d\n", thrno);
        for (i = 0; i < 0x100000; i++)
            ;
    }
}

int main()
{
    thread_t *t1, *t2;
    int t1no = 1, t2no = 2, j, i;
    t1 = thread_create(threadFunction, (void *)&t1no);
    t2 = thread_create(threadFunction, (void *)&t2no);

    for (;;)
    {
        printf("This is main thread\n");
        for (j = 0; j < 0x200000; j++)
            ;
    }
}
