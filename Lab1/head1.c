/////////////////////////////////////////////////////////////////////
// UNIX Commands prototype implementation
// Command: head
// Options supported: [<-nlines>] <file1> <file2> ...
// Authors: Dr Chapram Sudhakar, Dr Rashmi Ranjan Rout
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int head(int argc, char *argv[]);

char buffer[4096];

int main(int argc, char *argv[])
{
    head(argc, argv);
}

int head(int argc, char *argv[])
{

    //Sir Variables
    int fd;      // File descriptor
    int i, k;    // Index variables
    int nbytes;  // No of bytes read
    int len = 0; // Length of string (arg)
    int count = 0,
        num = 0; // For counting the number of lines

    if (argc == 1)
    {
        printf("Wrong number of arguments\nUsage:\n\t %s [<-nlines>] <file(s)>\n", argv[0]);
        return 0;
    }

    if (argv[1][0] == '-')
    {
        // Number of lines to be displayed
        num = atoi(&argv[1][1]);
        i = 2;
    }
    else
    {
        // Number of lines to be displayed by default 10
        num = 10;
        i = 1;
    }

    for (; i < argc; i++)
    {
        // Process ith argument file
        fd = open(argv[i], O_RDONLY);

        if (fd < 0)
        {
            sprintf(buffer, "Error: Opening file %s :", argv[i]);
            perror(buffer);
            continue;
        }
        count = 0;

        char ch;
        size_t index = 0, lines = 1;
        ssize_t rresult, wresult;

        // Read the contents from the file and display on stdout
        while ((rresult = read(fd, &ch, 1)) != 0) // Read 1 by 1 byte to fine "\n" new line
        {
            if (rresult < 0)
            {
                perror("read");
                return -1;
            }

            // Check if the current character is a new line (the line ends here)
            if (ch == '\n')
            {
                buffer[index] = ch;
                buffer[index + 1] = '\0';
                ch = 0;
                index = 0;

                // Print the line
                wresult = 0;
                ssize_t buffer_length = strlen(buffer);
                while (wresult != buffer_length)
                {
                    ssize_t res = write(STDOUT_FILENO, buffer + wresult, buffer_length - wresult);

                    if (wresult < 0)
                    {
                        perror("write");
                        return -1;
                    }

                    wresult += res;
                }

                // Check how many lines have to read
                if (lines == num)
                {
                    break;
                }

                lines++;
            }
            else
            {
                buffer[index++] = ch;
            }
        }

        if (close(fd) < 0)
        {
            perror("close");
            return -1;
        }
    }
    return (0);
}

//1. Use a library function to convert first count of lines argument, if one is given.
//2. Process incorrect arguments (not a proper number etc.)
//3. Process '-' command line argument or stdin.