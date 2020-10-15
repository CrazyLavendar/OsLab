/////////////////////////////////////////////////////////////////////
// UNIX shell prototype implementation
// Shell implementation with minimal features
// Authors: Dr Chapram Sudhakar, Dr Rashmi Ranjan Rout
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define MAXCOMMANDLEN 4096
#define MAXARGLEN 128
#define MAXARGCOUNT 256

char *com;
char args[MAXARGCOUNT][MAXARGLEN];
char *argv[MAXARGCOUNT];
char path[MAXCOMMANDLEN], spath[MAXCOMMANDLEN];
char fullpath[MAXCOMMANDLEN];

// The command line is in com[]. Number of characeters in the
// command line is len. It is parsed into separate arguments which
// are copied into args[][] array. The count of arguments
// including the command, is returned.
int parse_args(char com[], int len)
{
    int i, k, argcount = 0;

    // Skip initial blanks
    ...

        k = 0;
    while (i <= len)
    {
        if (com[i] == ' ' || com[i] == '\t' || com[i] == 0)
        {
            // End of the current argument
            ...

                // skip additional spaces
                ...
        }
        else
            args[argcount][k++] = com[i++];
    }

    return argcount;
}

int main()
{
    unsigned long int len = MAXCOMMANDLEN, alen;
    // Command line maximum length and actual length
    int countargs;    // Count of arguments
    int pid, status;  // Child process id and its termination status
    char *dirname;    // Pointer to directory name component
                      // of path environment variable.
    int aflag, rflag; // Absolute path or relative path
    int i, j;

    // Get the path envronment variable
    ...

        // Allocate memory for command line
        ...

        // Accept commands in an infinite loop
        while (1)
    {
        // Display the prompt and read the command.
        printf("SHELL $ ");
        alen = getline(&com, &len, stdin);

        // Replace the last newline with null character
        ...

            // If some command entered ...
            if (alen != 0)
        {
            // Parse the arguments
            countargs = parse_args(com, alen);

            // If the line is empty read next command
            ...

                // If exit command is entered, terminate the program
                ...

                // Point to those args,
                // skipping first one (base command)
                // and point to last NULL argument
                ...

                // Determine whether the command is an absolute or
                // relative path name
                aflag = 0;
            rflag = 0;
            ...

                // Copy the saved path environment variable
                ...

                // Not an absolute or relative path,
                // then search for the command in the
                // directories of the path
                if ((aflag == 0) && (rflag == 0))
            {
                // Search for the command in the path
                ...

                    // Append base command name to directory name
                    ...
                    // Check whether it is executable file
                    ...

                    // Otherwise check in the next directory
                    ...
            }
        }
        else
        {
            // Otherwise path of the command is given
            // either as absolute path or relative path
            ...
        }

        // Check if base command exists
        if ((argv[0] == NULL) || (access(argv[0], X_OK) != 0))
            printf("Error : No such command : '%s'\n", args[0]);
        else
        {
            // Create a child process and execute the
            // specified command.
            if ((pid = fork()) == 0)
            {
                // Child will execute the command
                ...
            }
            else
            {
                // parent will wait for the completion
                // of the command/child
                ...
            }
        }
    }
}

return 0;
}

// 1. Extend the program to process I/O redirection arguments. ( < input, > output, 2>error, 2>&1, >>output, 2>>error)
// 2. Extend the program to accept commands for background execution. ( command &). Signal handling support must be added to handle SIGCHLD signal.
// 3. Extend the program to support pipes. ( command1 | command2 )
// 4. Extend to take list of commands from a file.
//