//producer-consumer problem
//bounded-buffer problem-pthreads

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#define BUFFER_SIZE 20

int buffer[BUFFER_SIZE];
int counter = 0;

pthread_mutex_t mutex;
sem_t fillCount, emptyCount;

#define num 3
pthread_t producer[num];
pthread_t consumer[num];

void *producer_work(void *arg)
{
    int n = *(int *)arg;
    int i;
    while (1)
    {
        //produceItem
        sem_wait(&emptyCount);
        pthread_mutex_lock(&mutex);
        //putItemIntoBuffer(item);
        printf("\nProducer[%d] has put an item in buffer[%d]", n, counter);
        buffer[counter] = n; //putting its own id
        for (i = 0; i < 0x2000000; i++)
            ; //it is writing to buffer
        counter++;
        pthread_mutex_unlock(&mutex);
        sem_post(&fillCount);
    }
}

void *consumer_work(void *arg)
{
    int n = *(int *)arg;
    int i;
    while (1)
    {
        sem_wait(&fillCount);
        pthread_mutex_lock(&mutex);
        //item = removeItemFromBuffer();
        printf("\nConsumer[%d] has removed an item from buffer[%d]", n, counter - 1);
        counter--;
        for (i = 0; i < 0x2000000; i++)
            ; //it is reading from buffer
              //return this value buffer[counter] as val to be consumed
        pthread_mutex_unlock(&mutex);
        sem_post(&emptyCount);
        //consumeItem(item);
    }
}

void main()
{
    //initialize
    pthread_mutex_init(&mutex, NULL);
    sem_init(&fillCount, 0, 0);
    sem_init(&emptyCount, 0, BUFFER_SIZE);
    counter = 0;

    int i, ret;
    int arr[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    for (i = 0; i < num; i++)
    {
        ret = pthread_create(&producer[i], NULL, producer_work, (void *)&arr[i]);
        if (ret != 0)
            printf("Error in creating producer no %d :", i);
    }

    for (i = 0; i < num; i++)
    {
        ret = pthread_create(&consumer[i], NULL, consumer_work, (void *)&arr[i]);
        if (ret != 0)
            printf("Error in creating consumer no %d :", i);
    }

    for (i = 0; i < num; i++)
    {
        pthread_join(producer[i], NULL);
        pthread_join(consumer[i], NULL);
    }
}