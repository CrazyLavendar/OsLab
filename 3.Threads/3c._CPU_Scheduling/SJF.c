

#include <malloc.h>
#include <ucontext.h>
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#define MAXTHREADS 20
#define THREAD_STACKSIZE 32767
#define PUSER 40
sigset_t setg, osetg;
int FFS(int *);
void init_rq();
void printo(int *bm);
void printoq(void);
void initmap(int *bm);
void init_dq();

typedef struct thread_t
{
    int thr_id;

    int thr_pri;    //thr_pri	:current thread priority
    int thr_usrpri; //thr_usrpri	:user mode priority
    int thr_cpu;    //thr_cpu	:total cpu utilization
    int thr_nice;   //thr_nice	:nice val

    ucontext_t thr_context;
    void *thr_stack;
    int thr_stacksize;
    struct thread_t *thr_next;
    struct thread_t *thr_prev;
} thread_t;

thread_t thread_table[MAXTHREADS];
int ticks = 0;
int timeq = 0;
int bitmap[32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                  0, 0};

thread_t *ready_q = NULL, *ready_Q[32], *current_thread = NULL;
void timertick_handler(int alrm);
void schedule(void);

thread_t *thread_create(void (*thr_func)(void *), void *arg)
{
    int i;
    thread_t *thr;
    static int thrno = 1;
    struct itimerval t1;
    sigset_t set, oset;

    // Allocate thread structure.
    for (i = 0; i < MAXTHREADS; i++)
        if (thread_table[i].thr_id == 0) // Unused
            break;

    if (i >= MAXTHREADS)
        return NULL;

    thr = &thread_table[i];
    // Get the current execution context
    getcontext(&(thr->thr_context));

    // Modify the context to a new stack
    thr->thr_context.uc_link = 0;
    thr->thr_stack = thr->thr_context.uc_stack.ss_sp = malloc(THREAD_STACKSIZE);
    thr->thr_context.uc_stack.ss_size = THREAD_STACKSIZE;
    thr->thr_context.uc_stack.ss_flags = 0;
    if (thr->thr_context.uc_stack.ss_sp == 0)
    {
        perror("malloc: Could not allocate stack");
        exit(1);
    }

    // Create the new context--register the fnx
    printf("Creating child fiber\n");
    makecontext(&thr->thr_context, thr_func, 1, arg);

    // Initialize other fields of the thread structure
    thr->thr_id = thrno++;
    thr->thr_usrpri = 0;
    thr->thr_cpu = 0;
    thr->thr_nice = 10;
    thr->thr_stacksize = THREAD_STACKSIZE;

    //********************** TO BE DONE :: Block the signals during adding to the ready_q;
    sigfillset(&set);
    sigprocmask(SIG_SETMASK, &set, &oset);

    if (FFS(bitmap) == -1) //Ready Q is null
    {
        // Create the main thread also
        for (i = 0; i < MAXTHREADS; i++)
            if (thread_table[i].thr_id == 0)
            {
                current_thread = &thread_table[i];
                current_thread->thr_id = thrno++;
                current_thread->thr_usrpri = 40;
                current_thread->thr_cpu = 0;
                current_thread->thr_nice = 0;
                current_thread->thr_next = NULL;
                current_thread->thr_prev = NULL;
                ready_Q[current_thread->thr_usrpri / 4] = current_thread;
                bitmap[current_thread->thr_usrpri / 4] = 1;
                printf("\nThe main thread is assigned is %d", current_thread->thr_id);
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

    // Add this to ready q
    ready_q = ready_Q[thr->thr_usrpri / 4];
    thr->thr_next = ready_q;
    thr->thr_prev = NULL;
    if (ready_q != NULL)
        ready_q->thr_prev = thr;
    ready_q = thr;
    ready_Q[thr->thr_usrpri / 4] = ready_q;
    bitmap[thr->thr_usrpri / 4] = 1;
    printo(bitmap);

    //**********************DONE BY ME :: signals enabled after adding to the ready_q;
    sigprocmask(SIG_SETMASK, &oset, &set);

    return thr;
}

void thread_exit(void)
{
    // Remove this thread from the ready_q
    // TO BE DONE :: Block the signals during removing from the ready_q;
    sigfillset(&setg);
    sigprocmask(SIG_SETMASK, &setg, &osetg);

    if (current_thread->thr_next != NULL)
        (current_thread->thr_next)->thr_prev = current_thread->thr_prev;
    if (current_thread->thr_prev != NULL)
        (current_thread->thr_prev)->thr_next = current_thread->thr_next;

    if (current_thread == ready_Q[current_thread->thr_usrpri / 4])
        ready_Q[current_thread->thr_usrpri / 4] = current_thread->thr_next;
    free(current_thread->thr_stack);
    current_thread->thr_id = 0; // Free the slot
    if (ready_Q[current_thread->thr_usrpri / 4] == NULL)
        bitmap[current_thread->thr_usrpri / 4] = 0;
    current_thread = NULL;

    schedule();
    // DO NOT ENABLE SIGNALS HERE
}

void thread_yield(void)
{
    schedule();
}

void schedule(void)
{
    thread_t *t1, *t2;
    sigset_t sigt;
    struct itimerval tm;
    ucontext_t dummy;
    /*
	FFS first node/current_thread->next
	1. if thread xit     							RR	-VOULNTEERLY
	2. if yield(/block)  							RR	-VOULNTEERLY
	3. if timeq xpired select new thread current->next(if not NULL) else ready_Q starting thread = Round_Robin sched(RR)	-PREEMPT
	4. if higher priority process arrives					FFS then first thrd				-PREEMPT
	soln - check FFS if same as current process then follow -RR else if diffrent then select first in ready_Q[new]
	*/

    int ind = FFS(bitmap);
    //printo(bitmap);printoq();
    if (ind == -1) //this was last thread in system  and calld xit(cald sched)
    {
        if (current_thread == NULL) // No more threads? (stop itimer)
        {
            tm.it_interval.tv_usec = 0;
            tm.it_interval.tv_sec = 0;
            tm.it_value.tv_usec = 0; // ZERO Disable
            tm.it_value.tv_sec = 0;
            setitimer(ITIMER_PROF, &tm, NULL);
        }
        return;
    }
    //1.)this process calld xit and (no other thrd || or may hav mor thrd :on that Q)
    //if cur->thrd not don null : ind>(current_thread->thr_usrpri/4)[BELOW]

    if (current_thread == NULL || ind < (current_thread->thr_usrpri / 4)) //2.)hihr priority process arrived ||or this process moved to lower priority
    {
        t1 = ready_Q[ind]; //ind gets [ABOVE]
    }                      //ind>(current_thread->thr_usrpri/4) case also inc in this			//select first-prcess on this new  index ready-q
    else                   //RoundRobin in case ind==(current_thread->thr_usrpri/4)
    {
        if (current_thread->thr_next != NULL)
            t1 = current_thread->thr_next; //next process on same Q
        else
            t1 = ready_Q[current_thread->thr_usrpri / 4]; //start-1st process on same Q
    }

    //now swap the context
    if (current_thread != NULL)
    {
        t2 = current_thread;
        current_thread = t1;
        timeq = 0;
        sigemptyset(&sigt);
        sigaddset(&sigt, SIGPROF);
        sigprocmask(SIG_UNBLOCK, &sigt, NULL);
        swapcontext(&(t2->thr_context), &(current_thread->thr_context));
    }
    else //exited-only
    {
        current_thread = t1;
        timeq = 0;
        sigemptyset(&sigt);
        sigaddset(&sigt, SIGPROF);
        sigprocmask(SIG_UNBLOCK, &sigt, NULL);
        swapcontext(&(dummy), &(current_thread->thr_context));
    }
}

void timertick_handler(int alrm) //-----------------> clock intrpt handlr(   )  - every tick invoked
{
    thread_t *t1;
    int pri_Q1, pri_Q2;

    ticks++;
    current_thread->thr_cpu += 10; // One timer tick = 10 Units
    //current_thread->thr_cpupri += 10; 	// One timer tick
    timeq += 10;

    if (ticks % 4 == 0)
    {
        pri_Q1 = current_thread->thr_usrpri / 4;

        //TO DO: Re computing of the priority of the cuurent thread must be done. If necessary scheduler must be called.
        int newp = PUSER + (current_thread->thr_cpu / 4) + (2 * current_thread->thr_nice);
        if (newp <= 127)
            current_thread->thr_usrpri = newp;
        else
            current_thread->thr_usrpri = 127;

        pri_Q2 = (current_thread->thr_usrpri / 4);
        printf("\nPRI_Q1 : %d  nd newp :%d nd PRI_Q2 :%d nd curnt thrd id is:%d", pri_Q1, newp, pri_Q2, current_thread->thr_id);
        if (pri_Q1 != pri_Q2)
        {
            //DO :ready q changes put it to another q
            //call scheduler

            if (current_thread->thr_next != NULL)
                (current_thread->thr_next)->thr_prev = current_thread->thr_prev;
            if (current_thread->thr_prev != NULL)
                (current_thread->thr_prev)->thr_next = current_thread->thr_next;
            if (current_thread == ready_Q[pri_Q1])
                ready_Q[pri_Q1] = current_thread->thr_next;
            if (ready_Q[pri_Q1] == NULL)
                bitmap[pri_Q1] = 0;

            t1 = ready_Q[pri_Q2];
            if (t1 == NULL)
            {
                current_thread->thr_next = NULL;
                current_thread->thr_prev = NULL;
                ready_Q[pri_Q2] = current_thread;
                bitmap[pri_Q2] = 1;
            }
            else
            {
                while (t1->thr_next != NULL)
                {
                    t1 = t1->thr_next;
                }
                current_thread->thr_next = NULL;
                current_thread->thr_prev = t1;
                t1->thr_next = current_thread;
            }
            if (FFS(ready_Q) < (current_thread->thr_usrpri / 4))
                schedule();
        }
    }
    /*
	if (ticks % 25 == 0) 			//----------------->schedcpu() working embedded - every second(25 ticks) 
	{					//----------------->works as schedcpu() every 1 second
		thread_t *t2;

		int dummymap[32];
		initmap(dummymap);
		thread_t * dummy_Q[32]; 
		for(i=0;i<32;i++)//init_dq();
			{dummy_Q[i]=NULL;}
		
		for(i=0;i<32;i++)
		{
				t1 = ready_Q[i];
				while (t1 != NULL)
				{	
					t1->thr_cpu /= 2;										//1
					

					int newp=PUSER+(t1->thr_cpu /4)+(2*t1->thr_nice);
					if(newp<=127)
						t1->thr_usrpri=newp;									//2
					//else
						//t1->thr_usrpri=127;

					//DO: ready q changes put it to another q --but dont compute it again there	 		//3
					//call scheduler-- If necessary scheduler must be called					
					
					pri_Q2=t1->thr_usrpri/4;
					if(dummy_Q[pri_Q2]==NULL)
						{t1->thr_next=NULL;
						 t1->thr_prev=NULL;
						 dummy_Q[pri_Q2]=t1;}
					else
						{
						t2=dummy_Q[pri_Q2];
						while(t2->thr_next!=NULL)
							t2=t2->thr_next;
						t2->thr_next=t1;
						t1->thr_prev=t2;
						t1->thr_next=NULL;
						}	
					dummymap[pri_Q2]=1;
					t1 = t1->thr_next;
				}

		}
		for(i=0;i<32;i++)
		{bitmap[i]=dummymap[i];
		 ready_Q[i]=dummy_Q[i];}

		if(FFS(dummymap)<(current_thread->thr_usrpri/4))
			{schedule();}
	}
*/
    if (timeq >= 20)
    {
        printf("\nTime quantum expired!!!!");
        schedule();
    }
}

// The child thread will execute this function
void threadFunction(void *arg)
{
    int i, j, thrno;
    thrno = *(int *)arg;
    for (j = 0;; j++)
    {
        //printf("This is child thread : %d\n", thrno);
        for (i = 0; i < 0x100000; i++)
            ;
        //if(j==10){printf("\nBYE");thread_exit();}
    }
}

int main()
{
    init_rq();
    thread_t *t1, *t2;
    int t1no = 1, t2no = 2, j;
    t1 = thread_create(threadFunction, (void *)&t1no);
    t2 = thread_create(threadFunction, (void *)&t2no);

    for (;;)
    {
        //printf("This is main thread\n");
        for (j = 0; j < 0x200000; j++)
            ;
    }
}

int FFS(int *bm)
{
    int i;
    for (i = 0; i < 32; i++)
    {
        if (bm[i] == 1)
            return (i);
    }
    return (-1);
}

void printo(int *bm)
{
    int i;
    printf("\n");
    for (i = 0; i < 32; i++)
    {
        printf("%d", bm[i]);
    }
}
void initmap(int *bm)
{
    int i;
    for (i = 0; i < 32; i++)
    {
        bm[i] = 0;
    }
}

void printoq(void)
{
    int i;
    printf("\n");
    for (i = 0; i < 32; i++)
    {
        printf("HELL%u", ready_Q[i]);
    }
}

void init_rq()
{
    int i;
    for (i = 0; i < 32; i++)
    {
        ready_Q[i] = NULL;
    }
}

void init_dq()
{
    int i;
}