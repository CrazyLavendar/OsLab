#define CHAIRS 5
#define CUSTOMERS 10

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <semaphore.h>

sem_t mutex, sem_cust, sem_bbr;
int waiting, cur_cust;
int cqueue[CHAIRS + 1], f, r;

int isEmpty()
{
    return f == r;
}

int isFull()
{
    return (r + 1) % (CHAIRS + 1) == f;
}

void enqueue(int i)
{
    if (!isFull())
    {
        cqueue[r] = i;
        r = (r + 1) % (CHAIRS + 1);
    }
    else
        printf("queue is full\n");
}

int dequeue()
{
    if (!isEmpty())
    {
        int temp = cqueue[f];
        f = (f + 1) % (CHAIRS + 1);
        return temp;
    }
    else
        printf("queue is empty\n");

    return -1;
}

void *barber(void *arg)
{
    while (1)
    {
        sem_wait(&sem_cust);
        sem_wait(&mutex);
        waiting--;
        int i = dequeue();
        printf("Barber doing the cut for %d.\n", i);
        sem_post(&sem_bbr);
        sem_post(&mutex);
    }
}

void *customer(void *arg)
{
    sem_wait(&mutex);
    if (waiting < CHAIRS)
    {
        waiting++;
        enqueue(*(int *)arg);
        printf("Customer %d waiting.\n", *(int *)arg);
        sem_post(&sem_cust);
        sem_post(&mutex);
        sem_wait(&sem_bbr);
        //printf("Cut done for Customer %d\n",*(int *)arg);
    }
    else
    {
        printf("No waiting chairs available.. Customer %d left.\n", *(int *)arg);
        sem_post(&mutex);
    }
}

int main(void)
{
    pthread_t customers[CUSTOMERS];
    pthread_t bbr;
    int s, i, arg[CUSTOMERS];
    f = 0;
    r = 0;
    waiting = 0;
    sem_init(&mutex, 0, 1);
    sem_init(&sem_bbr, 0, 0);
    sem_init(&sem_cust, 0, 0);

    for (i = 0; i < CUSTOMERS; i++)
    {
        arg[i] = i + 1;
        if (0 != (s = pthread_create(&customers[i], NULL, &customer, &arg[i])))
        {
            perror("Error creating thread");
            exit(EXIT_FAILURE);
        }
    }

    if (0 != (s = pthread_create(&bbr, NULL, &barber, NULL)))
    {
        perror("Error creating thread");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < CUSTOMERS; i++)
    {
        if (0 != (s = pthread_join(customers[i], NULL)))
        {
            perror("Error joining thread");
            exit(EXIT_FAILURE);
        }
    }

    if (0 != (s = pthread_join(bbr, NULL)))
    {
        perror("Error joining thread");
        exit(EXIT_FAILURE);
    }

    sem_destroy(&mutex);
    sem_destroy(&sem_cust);
    sem_destroy(&sem_bbr);

    exit(EXIT_SUCCESS);
}
