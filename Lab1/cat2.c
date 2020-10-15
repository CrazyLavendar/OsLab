#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int cat(int argc, char *argv[]);

char buffer[4096]; // Buffer for reading/writing the file data

int main(int argc, char *argv[])
{
    return cat(argc, argv);
}

int cat(int argc, char *argv[])
{
    int i, j;               // Loop index variables
    int fd, count, fdwrite; // Open file descriptor and
    count = sizeof(buffer); // count of bytes to read/write

    if (argc == 1)
    {
        // No argument file, then display input from stdin
        while (fgets(buffer, 4096, stdin) != NULL)
            fputs(buffer, stdout);
        return 0;
    }

    for (j = 1; j < argc; j++)
        if (strcmp(argv[j], ">") == 0)
            fdwrite = open(argv[j + 1], O_WRONLY);

    if (fdwrite < 0)
    {
        sprintf(buffer, "Error: Opening output file %s :", argv[i]);
        perror(buffer);
    }

    for (i = 1; i < argc; i++)
    {

        if (strcmp(argv[i], "-") == 0)
        {
            // If "-" is specified as argument it refers to stdin
            while (fgets(buffer, 4096, stdin) != NULL)
                fputs(buffer, stdout);
            return 0;
        }
        else if (strcmp(argv[i], ">") == 0)
        {
            close(fdwrite);
            return 0;
        }
        else
        {
            // Open the ith argument file for reading
            fd = open(argv[i], O_RDONLY);
        }

        if (fd < 0)
        {
            sprintf(buffer, "Error: Opening file %s :", argv[i]);
            perror(buffer);
            continue;
        }

        // Read the contents from the file and display on stdout
        while (read(fd, buffer, sizeof(buffer)))
            write(fdwrite, buffer, sizeof(buffer));

        // If current file is not stdin, then close the file
        close(fdwrite);
    }

    return 0;
}