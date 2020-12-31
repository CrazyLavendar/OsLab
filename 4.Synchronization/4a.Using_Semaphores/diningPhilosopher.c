//dining philosphers problem-INTER_PROCESS_COMMUNICATION

#include <stdlib.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#define num 5

void main(int argc, char *argv[])
{
    int i; /*
	sem_unlink("/chopstick1");
	sem_unlink("/chopstick2");
	sem_unlink("/chopstick3");
	sem_unlink("/chopstick4");
	sem_unlink("/chopstick0");
	scanf("%d",&i);*/
    sem_t *left, *right;
    int n;
    if (argc == 2)
        n = atoi(argv[1]);
    else
        printf("\nUsage is <./a.out> <philospher_num> ");

    char str1[20] = "/chopstick";
    strcat(str1, argv[1]);
    puts(str1);

    char str2[20] = "/chopstick";
    //strcat(str2,(char*)(n+1));
    sprintf(str2, "%s%d", str2, (n + 1) % num);
    puts(str2);

    left = sem_open(str1, O_CREAT, S_IRUSR | S_IWUSR, 1);
    right = sem_open(str2, O_CREAT, S_IRUSR | S_IWUSR, 1);
    if (left == SEM_FAILED || right == SEM_FAILED)
        printf("\nError opening sem");
    else
        printf("\nSem successfully acquired");
    //sem_close(left);
    //sem_close(right);
    //scanf("%d",&i);
    while (1)
    {
        //think
        if (n % 2 == 0) //even
        {
            sem_wait(left);
            sem_wait(right);
        }
        else
        {
            sem_wait(right);
            sem_wait(left);
        }

        //eat
        printf("\n Philospher[%d] eating", n);
        for (i = 0; i < 0x1000000; i++)
            ; //it is eating

        sem_post(left);
        sem_post(right);
        printf("\n Philospher[%d] eating OVER", n);
    }
}

//NOTE:
/*
1. mutual xclusion
2. progress
3. bounded waiting time(based on scheduling policy used to wake up a(more) on mutex_unlock) 

->no starvation??*/