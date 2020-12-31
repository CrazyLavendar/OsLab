#define SIZE 500000

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
        //printf("left=%d,right=%d\n",left,right);
        while (a[left] <= key && left <= high)
            left++;

        while (a[right] > key && right >= low)
            right--;

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
    //printf("pivot=%d",pivot);
    return pivot;
}

int main(void)
{
    int i = 0;
    int arr[SIZE];

    //fill array
    for (i = 0; i < SIZE; i++)
    {
        arr[i] = random() % SIZE;
    }

    //printArray(arr,SIZE);
    struct timespec t1, t2;
    clock_gettime(CLOCK_MONOTONIC, &t1);

    quicksort(arr, 0, SIZE - 1);

    clock_gettime(CLOCK_MONOTONIC, &t2);

    printf("Time taken to compute sort %d using simple quicksort(nano secs) : %ld\n", SIZE, (t2.tv_nsec - t1.tv_nsec));

    return 0;
}