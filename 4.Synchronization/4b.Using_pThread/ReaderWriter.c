#define MAX 1000
#define WRITERS 10
#define READERS 10

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>

char buffer[MAX];
pthread_mutex_t lock, w, r;
pthread_cond_t wc, rc;
int writect = 0, pendingWriters = 0;
int readct = 0, pendingReaders = 0;

void *doWrite(void *arg)
{

    //	pthread_mutex_lock(&r);
    pthread_mutex_lock(&w);
    int i;
    //CS
    //	int i=random()%100,j=random()%100;

    //	for(;i>=0;i--)
    //		for(;j>=0;j++);

    for (i = 0; i < 0x3000000; i++)
        ;
    printf("%d is Writting\n", *(int *)arg);

    pthread_mutex_unlock(&w);
    //	pthread_mutex_unlock(&r);
}

void *doRead(void *arg)
{

    //pthread_mutex_lock(&r);
    pthread_mutex_lock(&lock);
    readct++;
    if (readct == 1)
    {
        pthread_mutex_lock(&w);
    }
    pthread_mutex_unlock(&lock);
    //pthread_mutex_unlock(&r);
    int i;
    //CS
    //	int i=random()%110,j=random()%100;

    //	for(;i>=0;i--)
    //		for(;j>=0;j++);
    for (i = 0; i < 0x3000000; i++)
        ;
    printf("%d is Reading\n", *(int *)arg);

    pthread_mutex_lock(&lock);
    readct--;
    if (readct == 0)
    {
        pthread_mutex_unlock(&w);
    }
    pthread_mutex_unlock(&lock);
}

int main(void)
{

    pthread_t writers[WRITERS], readers[READERS];
    int writerarg[WRITERS], readerarg[READERS];

    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&w, NULL);
    pthread_cond_init(&wc, NULL);
    pthread_mutex_init(&r, NULL);
    pthread_cond_init(&rc, NULL);

    int i, s;
    for (i = 0; i < WRITERS; i++)
    {
        writerarg[i] = i + 1;
        if (0 != (s = pthread_create(&writers[i], NULL, &doWrite, &writerarg[i])))
        {
            perror("Error creating thread");
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < READERS; i++)
    {
        readerarg[i] = i + 1;
        if (0 != (s = pthread_create(&readers[i], NULL, &doRead, &readerarg[i])))
        {
            perror("Error creating thread");
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < WRITERS; i++)
    {
        if (0 != (s = pthread_join(writers[i], NULL)))
        {
            perror("Error joining thread");
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < READERS; i++)
    {
        if (0 != (s = pthread_join(readers[i], NULL)))
        {
            perror("Error joining thread");
            exit(EXIT_FAILURE);
        }
    }

    pthread_mutex_destroy(&lock);
    pthread_mutex_destroy(&w);
    pthread_mutex_destroy(&r);
    pthread_cond_destroy(&rc);
    pthread_cond_destroy(&wc);

    exit(EXIT_SUCCESS);
}