#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#define MAX 5

sem_t mutex, br, cs;
int N = 0;

void *barber(void *ptr)
{
    do
    {
        sem_wait(&cs);
        sem_wait(&mutex);

        N--;
        printf("Barber has serviced one customer,no. of customers left to be serviced is=%d\n", N);

        sem_post(&mutex);

        /*if(N==0)
	{
		while(N==0);
			
	}*/
        sem_post(&br);
    } while (1);
}

void *customer(void *ptr)
{
    do
    {
        sem_wait(&mutex);

        if (N <= MAX)
        {
            N++;
            printf("The no. of customers waiting is %d\n", N);

            sem_post(&mutex);
            sem_post(&cs);
            sem_wait(&br);
        }

        sem_post(&mutex);
    } while (1);

    return 0;
}

int main()
{
    pthread_t brbr, cust;
    pthread_create(&brbr, NULL, barber, NULL);
    pthread_create(&cust, NULL, customer, NULL);

    sem_init(&mutex, 0, 1);
    sem_init(&br, 0, 1);
    sem_init(&cs, 0, 0);

    pthread_join(brbr, NULL);
    pthread_join(cust, NULL);

    sem_destroy(&mutex);
    sem_destroy(&br);
    sem_destroy(&cs);

    return 0;
}