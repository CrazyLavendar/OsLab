/////////////////////////////////////////////////////////////////////
// UNIX Commands prototype implementation
// Command: rm
// Options supported: <file1> <file2> ...
// Authors: Dr Chapram Sudhakar, Dr Rashmi Ranjan Rout
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <unistd.h>
#include <string.h>

int rdir(int argc, char *argv[]);
int rm(int argc, char *argv[]);

char buffer[512]; // For error message printing

int main(int argc, char *argv[])
{
    rm(argc, argv);
}

int rm(int argc, char *argv[])
{
    int fd = 0;
    int i;
    char c;

    if (argc < 2)
    { /* allowing only single argument */
        printf("Invalid number of Arguments\n");
        printf("Usage:\n\trm <file>\n\n");
        return (0);
    }

    //if (strcmp(argv[i], "") != 0)
    // {
    // Confirm, whether really to be removed
    printf("Are you really want to delete \n");
    printf("Enter 'y' or 'Y' to delete. Else enter any key to skip \n");

    c = getchar();

    if ((c == 'y') || (c == 'Y'))
    {
        for (i = 1; i < argc; i++)
        {

            remove(argv[i]);
        }
    }
    //}

    return (0);
}

//1. Extend the program to accept -r option and remove directory (directories) recursively.
//2. Extend the program to support -i option to prompt before deletion. In the present
//implementation without -i, it is handling as if -i option is given.
//3. Extend the program to support -f, forceful removal of an entry.