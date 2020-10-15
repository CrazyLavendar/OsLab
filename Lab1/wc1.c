/////////////////////////////////////////////////////////////////////
// UNIX Commands prototype implementation
// Command: wc
// Options supported: <noargs>, <file1> <file2> ...
// Authors: Dr Chapram Sudhakar, Dr Rashmi Ranjan Rout
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

int wc(int argc, char *argv[]);

int main(int argc, char **argv, char **envp)
{
    return wc(argc, argv);
}

char buffer[4096];

int wc(int argc, char *argv[])
{
    int fd;                     // File descriptor
    int lines, words, chars;    // No of lines, words, characters
    int tlines, twords, tchars; // Total no of lines, words, characters
    int i, j;                   // Loop index variables
    char c;                     // Character read
    char t;                     // Previous character, to remember space.
    int stop = 0;
    int count;

    // If no arguments are entered
    if (argc < 2)
    {
        lines = words = chars = 0;
        t = ' '; // Previous space character
        while ((c = getchar()) != (-1))
        {
            chars++;

            if (isspace(c))
            {
                // Skip all white spaces

                // If previously remembered character is not
                // space increment the word count.
            }
            //
        }
        words++;
        printf("\t%d\t%d\t%d\n", lines, words, chars);
        return (0);
    }

    // From files ...
    tlines = twords = tchars = 0;
    for (i = 1; i < argc; i++)
    {
        // Process ith file
        fd = open(argv[i], O_RDONLY, 0);

        if (fd < 0)
        {
            sprintf(buffer, "Error : opening file %s ", argv[i]);
            perror(buffer);
            continue;
        }

        lines = words = chars = 0;
        t = ' '; // Previous space character
        while ((count = read(fd, buffer, sizeof(buffer))) > 0)
        {
            //
        }

        close(fd);
        printf("%s\t%d\t%d\t%d\n", argv[i], lines, words, chars);

        // Accumulate the total counts
        tlines += lines;
        twords += words;
        tchars += chars;
    }

    printf("Total\t%d\t%d\t%d\n", tlines, twords, tchars);
    return (0);
}

//1. Is the program considering multiple consecutive spaces as one separator or each space is
//counted as one word separator?
//2. Check whether tab and new line characters are considered as separators.
//3. Modify the program to count only lines, only words and only characters using command
//line options for each one of them.
//4. Extend the program to redirect the output into a file using command line option.
//5. Modify the program to read one block (4096 bytes) at a time from the file and process the
//data in the buffer.