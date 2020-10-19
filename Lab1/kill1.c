/////////////////////////////////////////////////////////////////////
// UNIX Commands prototype implementation
// Command: kill
// Options supported: [-signo] pid1 pid2 ...
// Authors: Dr Chapram Sudhakar, Dr Rashmi Ranjan Rout
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int killp(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage:\n\t %s  pid1 <pid2> <pid3> ...\n", argv[0]);
        exit(0);
    }

    killp(argc, argv);
}

int killp(int argc, char *argv[])
{
    int i, sig, retval, p;
    char buf[100];

    // Find the first argument, whether it is a signal
    if (argv[1][0] == '-')
    {
        printf("With - %s\n", &argv[1][1]);
        sig = atoi(&argv[1][1]);
        i = 2;
    }
    else
    {
        sig = SIGKILL; // Otherwise default interrupt signal
        i = 1;
    }

    // Send the signal all processes given
    for (; i < argc; i++)
    {
        kill(atoi(argv[i]), sig);
    }

    return 0;
}

//1. Extend the program to process -s, -l and -L arguments.
//2. Extend the program to accept special PID values such as group IDs or all processes (-1) etc.