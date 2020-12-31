//reader-writers problem using semaphores-INTER_PROCESS_COMMUNICATION

#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <semaphore.h>
#define BUFFER_SIZE 100

void main(int argc, char *argv[])
{
    int i, counter, n;
    sem_t *zreaders, *zwriters, *rmutex, *nreaders;
    if (argc == 2)
        n = atoi(argv[1]);
    else
    {
        printf("\nUsage is <./a.out> <philospher_num> ");
        return;
    }

    //when this code xecuted fst-time:creates+attatches    and   rest-time:only attatches  "the shared memory region"
    key_t key = 115;
    int shmid;
    if ((shmid = shmget(key, (size_t)(BUFFER_SIZE * 2), IPC_CREAT | O_RDWR | 0666)) < 0) //for 10 integers but will be rounded up to multiple_of_pages
        printf("\nError in getting shared memory region");
    char *buffer;
    if ((buffer = (char *)shmat(shmid, (void *)0, O_RDONLY)) == (char *)-1)
        printf("\nFailed to attatch Shared Memory Segment");
    else
        printf("\nSuccessfully attatch Shared Memory Segment");

    //when this code xecuted fst-time:creates+gets    and   rest-time:only gets  "the named semaphore"
    zreaders = sem_open("/zreaders", O_CREAT, S_IRUSR | S_IWUSR, 1);
    zwriters = sem_open("/zwriters", O_CREAT, S_IRUSR | S_IWUSR, 1);
    rmutex = sem_open("/rmutex", O_CREAT, S_IRUSR | S_IWUSR, 1);
    nreaders = sem_open("/nreaders", O_CREAT, S_IRUSR | S_IWUSR, 0);

    //critical-section problem
    int prev, curr;
    while (1)
    {
        sem_wait(zwriters);
        sem_wait(rmutex);
        sem_getvalue(nreaders, &prev);
        sem_post(nreaders);
        sem_post(rmutex);
        if (prev == 0)
            sem_wait(zreaders);
        sem_post(zwriters);
        printf("\nI am Reader reading buffer and the contents read are :");
        printf("%s", buffer);
        for (i = 0; i < 0x1000000; i++)
            ; //it is reading

        sem_wait(rmutex);
        sem_wait(nreaders);
        sem_getvalue(nreaders, &curr);
        sem_post(rmutex);
        if (curr == 0)
            sem_post(zreaders);
    }
}