//producer-consumer problem
//bounded-buffer problem-INTER_PROCESS_COMMUNICATION

#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>

#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#define BUFFER_SIZE 10

void main(int argc, char *argv[])
{
    int i, counter, n;
    sem_t *mutex, *fillCount, *emptyCount, *Counter;
    if (argc == 2)
        n = atoi(argv[1]);
    else
    {
        printf("\nUsage is <./a.out> <philospher_num> ");
        return;
    }

    //when this code xecuted fst-time:creates+attatches    and   rest-time:only attatches  "the shared memory region"
    key_t key = 114;
    int shmid;
    if ((shmid = shmget(key, (size_t)(BUFFER_SIZE * 2), IPC_CREAT | O_RDWR | 0666)) < 0) //for 10 integers but will be rounded up to multiple_of_pages
        printf("\nError in getting shared memory region");
    int *buffer;
    if ((buffer = (int *)shmat(shmid, (void *)0, 0)) == (int *)-1)
        printf("\nFailed to attatch Shared Memory Segment");
    else
        printf("\nSuccessfully attatch Shared Memory Segment");

    //when this code xecuted fst-time:creates+gets    and   rest-time:only gets  "the named semaphore"
    mutex = sem_open("/mutex", O_CREAT, S_IRUSR | S_IWUSR, 1);
    fillCount = sem_open("/fillCount", O_CREAT, S_IRUSR | S_IWUSR, 0);
    emptyCount = sem_open("/emptyCount", O_CREAT, S_IRUSR | S_IWUSR, BUFFER_SIZE);
    Counter = sem_open("/Counter", O_CREAT, S_IRUSR | S_IWUSR, 0);

    //counter - let it be a variable after buffer  --say 20B buffer(=10 integer vals:0 to 9 index)
    //					       --say 10 index be counter val

    //critical-section problem
    while (1)
    {
        sem_wait(fillCount);
        sem_wait(mutex);
        sem_wait(Counter);
        sem_getvalue(Counter, &counter);
        printf("\nConsumer[%d] has removed an item from buffer[%d]", n, counter);
        //return buffer[counter] //putting its own id o/w random no. can be kept in buffer
        sem_post(mutex);
        sem_post(emptyCount);
    }
}
