#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
void SIGINThandler(int signo)
{
    static int i = 0;
    i++;
    if (i == 3)
    {
        printf("\nI am dying\n");
        exit(0);
    }
    else
        printf("\nWhy do you want to stop me\n");
}

void main()
{
    signal(SIGINT, SIGINThandler);
    while (1)
        ;
    return;
}