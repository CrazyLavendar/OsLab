#define SIZE 500000
#define NTHREADS 32

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>

int arr[SIZE];

struct range
{
    int start, end;
};

void printArray(int *a, int size)
{
    int i = 0;
    for (i = 0; i < size; i++)
        printf("%d ", a[i]);
    printf("\n");
}

void quicksort(int *a, int start, int end)
{
    //printf("start=%d,end=%d\n",start,end);
    if (start < 0 || end < 0 || start >= end)
        return;

    int pivot = partition(a, start, end);
    //printf("pivot = %d\n",pivot);
    quicksort(a, start, pivot - 1);
    quicksort(a, pivot + 1, end);
}

int partition(int *a, int low, int high)
{

    int left, right, pivot;
    left = low;
    right = high;
    int key = a[low], temp;

    while (left < right)
    {
        //printf("left=%d,right=%d,a[%d]=%d,key=%d\n",left,right,left,a[left],key);
        while (a[left] <= key && left <= high)
            left++;

        //printf("left=%d",left);
        while (a[right] > key && right >= low)
            right--;
        //printf("\nright=%d",right);
        if (left < right)
        {
            temp = a[left];
            a[left] = a[right];
            a[right] = temp;
        }
    }

    temp = a[right];
    a[right] = key;
    pivot = right;
    a[low] = temp;

    return pivot;
}

static void *compute(void *arg)
{
    struct range *r = (struct range *)arg;

    quicksort(arr, r->start, r->end);
}

static void *computeHyperSort(void *arg)
{
    int s, i;

    struct range r[NTHREADS];
    pthread_t threads[NTHREADS];

    for (i = 0; i < NTHREADS; i++)
    {
        r[i].start = i * (SIZE / NTHREADS);
        r[i].end = r[i].start + SIZE / NTHREADS - 1;

        if (0 != (s = pthread_create(&threads[i], NULL, compute, &r[i])))
        {
            errno = s;
            perror("Could not create thread");
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < NTHREADS; i++)
    {
        if (0 != (s = pthread_join(threads[i], NULL)))
        {
            errno = s;
            perror("Could not create thread");
            exit(EXIT_FAILURE);
        }
    }

    int median = arr[(r[0].end - r[0].start + 1) / 2];
}

int main(void)
{
    int i = 0, s;
    pthread_t thread_id[NTHREADS];
    struct range r[NTHREADS];

    pthread_t th;
    struct range rg;

    void *ret;
    //fill array
    for (i = 0; i < SIZE; i++)
    {
        arr[i] = random() % SIZE;
    }
    //printArray(arr,SIZE);

    struct timespec t1, t2;
    clock_gettime(CLOCK_MONOTONIC, &t1);

    /*for(i=0;i<NTHREADS;i++){
		r[i].start = i*(SIZE/NTHREADS);
		r[i].end = r[i].start + SIZE/NTHREADS-1;
		if(0 != (s=pthread_create(&thread_id[i],NULL,compute,&r[i]))){
			errno = s;
			perror("Could not create thread");
			exit(EXIT_FAILURE);
		}
		//printf("(%d,%d) \n",r[i].start,r[i].end);
	}


	for(i=0;i<NTHREADS;i++){
		s = pthread_join(thread_id[i],NULL);
		if(s!=0){
			errno = s;
			perror("Could not create thread");
			exit(EXIT_FAILURE);
		}
	}*/

    rg.start = 0;
    rg.end = SIZE - 1;
    if (0 != (s = pthread_create(&th, NULL, compute, &rg)))
    {
        errno = s;
        perror("Could not create thread");
        exit(EXIT_FAILURE);
    }
    s = pthread_join(th, NULL);
    if (s != 0)
    {
        errno = s;
        perror("Could not create thread");
        exit(EXIT_FAILURE);
    }

    clock_gettime(CLOCK_MONOTONIC, &t2);

    //printArray(arr,SIZE);

    printf("Time taken to compute sort %d elements using threaded quicksort(nano secs) : %ld\n", SIZE, (t2.tv_nsec - t1.tv_nsec));

    return 0;
}