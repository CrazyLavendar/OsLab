// Coroutines
// Warnings might arise because of type conversion. Ignore it
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

#define MAXROUTINES 4
#define handle_error(msg)   \
    do                      \
    {                       \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)
#define STACK_SIZE 102400
ucontext_t contexts[MAXROUTINES], contexts_main;

void CoroutineA(void *arg);
void CoroutineB(void *arg);
void CoroutineC(void *arg);
void CoroutineD(void *arg);

int main()
{
    int i;

    // Initialize all context structures to execute
    if (getcontext(&contexts[0]) == -1)
        handle_error("getcontext 0 fails");
    contexts[0].uc_stack.ss_sp = malloc(STACK_SIZE);
    contexts[0].uc_stack.ss_size = STACK_SIZE;
    contexts[0].uc_link = &contexts[1];
    makecontext(&contexts[0], CoroutineA, 0);

    if (getcontext(&contexts[1]) == -1)
        handle_error("getcontext 1 fails");
    contexts[1].uc_stack.ss_sp = malloc(STACK_SIZE);
    contexts[1].uc_stack.ss_size = STACK_SIZE;
    contexts[1].uc_link = &contexts[2];
    makecontext(&contexts[1], CoroutineB, 0);

    if (getcontext(&contexts[2]) == -1)
        handle_error("getcontext 2 fails");
    contexts[2].uc_stack.ss_sp = malloc(STACK_SIZE);
    contexts[2].uc_stack.ss_size = STACK_SIZE;
    contexts[2].uc_link = &contexts[3];
    makecontext(&contexts[2], CoroutineC, 0);

    if (getcontext(&contexts[3]) == -1)
        handle_error("getcontext 3 fails");
    contexts[3].uc_stack.ss_sp = malloc(STACK_SIZE);
    contexts[3].uc_stack.ss_size = STACK_SIZE;
    contexts[3].uc_link = &contexts[0];
    makecontext(&contexts[3], CoroutineD, 0);

    // void , CoroutineB, ...

    // Do context switch to void

    printf("Initiate context switch to A\n");
    swapcontext(&contexts_main, &contexts[0]);

    printf("Exiting\n");

    // for (;;)
    //     ;

    return 0;
}

void CoroutineA(void *arg)
{
    int count = 0;
    while (1)
    {
        // Execute a delay loop doing nothing
        sleep(1);
        // Display count value
        printf("Couroutine A running %d times\n", ++count);

        //Limiting how many times to switch
        if (count == 7)
            contexts[0].uc_link = &contexts_main;
        // context switch to next coroutine
        swapcontext(&contexts[0], contexts[0].uc_link);
    }
}
void CoroutineB(void *arg)
{
    int count = 0;
    while (1)
    {
        // Execute a delay loop doing nothing
        sleep(1);
        // Display count value
        printf("Couroutine B running %d times\n", ++count);
        //Limiting how many times to switch
        if (count == 3)
            contexts[0].uc_link = &contexts[2];
        // context switch to next coroutine
        swapcontext(&contexts[1], contexts[1].uc_link);
    }
}
void CoroutineC(void *arg)
{
    int count = 0;

    while (1)
    {
        // Execute a delay loop doing nothing
        sleep(1);
        // Display count value
        printf("Couroutine C running %d times\n", ++count);
        //Limiting how many times to switch
        if (count == 5)
            contexts[0].uc_link = &contexts[0];
        // context switch to next coroutine
        swapcontext(&contexts[2], contexts[2].uc_link);
    }
}
void CoroutineD(void *arg)
{
    int count = 0;
    while (1)
    {
        // Execute a delay loop doing nothing
        sleep(1);
        // Display count value
        printf("Couroutine D running %d times\n", ++count);
        //Limiting how many times to switch
        if (count == 4)
            contexts[2].uc_link = &contexts[0];
        // context switch to next coroutine
        swapcontext(&contexts[3], contexts[3].uc_link);
    }
}