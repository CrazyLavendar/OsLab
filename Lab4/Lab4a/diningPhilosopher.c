//dining philosphers problem

#include <stdlib.h>
#include <pthread.h>
#define num 5 //dont change

pthread_mutex_t chopstick[5] = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER};
pthread_t philospher[num];

void *philospher_work(void *arg)
{
    int i;
    int n = *(int *)arg;
    while (1)
    {
        //think
        if (n % 2 == 0) //even
        {
            pthread_mutex_lock(&chopstick[n]);
            pthread_mutex_lock(&chopstick[(n + 1) % num]);
        }
        else
        {
            pthread_mutex_lock(&chopstick[(n + 1) % num]);
            pthread_mutex_lock(&chopstick[n]);
        }

        //eat
        printf("\n Philospher[%d] eating", n);
        for (i = 0; i < 0x1000000; i++)
            ; //it is eating

        pthread_mutex_unlock(&chopstick[n]);
        pthread_mutex_unlock(&chopstick[(n + 1) % num]);
        printf("\n Philospher[%d] eating OVER", n);
    }
}

void main()
{
    int i, ret;
    int arr[5] = {0, 1, 2, 3, 4};

    for (i = 0; i < num; i++)
    {
        ret = pthread_create(&philospher[i], NULL, philospher_work, (void *)&arr[i]);
        if (ret != 0)
            printf("Error in creating thread no %d :", i);
    }
    for (i = 0; i < num; i++)
        pthread_join(philospher[i], NULL);
}

//NOTE:
/*
1. mutual xclusion
2. progress
3. bounded waiting time(based on scheduling policy used to wake up a(more) on mutex_unlock) 

->no starvation??*/