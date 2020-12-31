/////////////////////////////////////////////////////////////////////
// UNIX Commands prototype implementation
// Command: cat
// Options supported: <noargs>, -, <file1> <file2> ...
// Authors: Dr Chapram Sudhakar, Dr Rashmi Ranjan Rout
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <unistd.h> /*for STDOUT_FILENO*/
#include <fcntl.h>  /*for open*/
#include <string.h> // To read string char

int cat(int argc, char *argv[]);

char buffer[4096]; // Buffer for reading/writing the file data

int main(int argc, char *argv[])
{
    return cat(argc, argv);
}

int cat(int argc, char *argv[])
{

    int i, j;               // Loop index variables
    int fd, count;          // Open file descriptor and
    count = sizeof(buffer); // count of bytes to read/write

    if (argc == 1)
    {
        // No argument file, then display input from stdin
        while (fgets(buffer, 4096, stdin) != NULL)
            fputs(buffer, stdout);
        return 0;
    }

    for (i = 1; i < argc; i++)
    {

        if (strcmp(argv[i], "-") == 0)
        {
            // If "-" is specified as argument it refers to stdin
            while (fgets(buffer, 4096, stdin) != NULL)
                fputs(buffer, stdout);
        }

        else if (strcmp(argv[i], ">") == 0)
        {
            i++;
            fd = open(argv[i], O_CREAT | O_RDWR, 0777);
            // Not mentioning what to read. By now contents should be in Buffer. Else the usage is wrong
            // Directly writing what is present in Buffer
            write(fd, buffer, count);
            close(fd);
        }

        else
        {

            // Open the ith argument file for reading
            fd = open(argv[i], O_RDONLY);

            if (fd < 0)
            {
                sprintf(buffer, "Error: Opening file %s :", argv[i]);
                perror(buffer);
                continue;
            }

            // Read the contents from the file and display on stdout 333
            while (read(fd, buffer, count))

                write(STDOUT_FILENO, buffer, count);
            // If current file is not stdin, then close the file
            close(fd);
        }
    }
    return 0;
}

//What is the reason for, returning less number of bytes than requested, by a read system call?
//Ans : Not to exceed stack overflow error
//Under what circumstances write system call fails?
//Ans : file not present ,invalid file
//What is the return value of fgets library function?
//Ans : 1 , 0 or -1
//Extend the given program code to redirect the output to a file. ( > output)
//Implemented
//Extend the given program code to redirect the standard input from a file. ( < input)