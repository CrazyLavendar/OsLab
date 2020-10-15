#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int nread(char *filename, int start, int size, char buf[]);

int main(int argc, char *argv[])
{
    char *filename;
    int start, size, n;
    char buf[8192];

    if (argc != 4)
    {
        fprintf(stderr, "Usage: nread FILENAME START SIZE\n");
        exit(1);
    }

    filename = argv[1];
    start = atoi(argv[2]);
    size = atoi(argv[3]);

    if (size > sizeof(buf))
    {
        fprintf(stderr, "nread: size %d too large\n", size);
        exit(1);
        /* Could just shrink the size. EG: size = sizeof(buf); */
    }

    n = nread(filename, start, size, buf);
    if (n > 0)
    {
        /*
     * Print the n bytes in buf[] on the standard output
     * file descriptor (which is 1).
     */
        if (write(1, buf, n) != n)
        {
            perror("nread: write");
            exit(1);
        }
    }

    exit(0);
}

/*
 * nread(filename, start, size, buf) -> #-bytes-read
 * Read size bytes from the file filename, starting at offset
 * start. Put them in buf, which must be at least size
 * bytes long. Return the number of characters actually
 * read, which could be less than size if start is near
 * the end of the file. Returns -1 if the file doesn't exist,
 * and 0 if start is at or beyond the end of the file.
 */
int nread(char *filename, int start, int size, char buf[])
{
    int fd, n;

    /*
   * Ask the UNIX kernel to "open" the file, returning a file descriptor
   * which we can use to get at the file contents.
   */
    fd = open(filename, O_RDONLY);
    if (fd < 0)
    {
        /* The file doesn't exist, or we're not allowed to read it. */
        perror(filename);
        return (-1);
    }

    /*
   * Ask UNIX to set the offset within the file from which it
   * will read data when next we call read().
   */
    if (lseek(fd, (off_t)start, SEEK_SET) < 0)
    {
        /*
     * Don't return an error, just pretend (perhaps correctly)
     * that we've reached the end of the file.
     */
        close(fd);
        return (0);
    }

    /*
   * Ask UNIX to read up to size bytes from file descriptor
   * fd into buf. read() returns the number of bytes actually
   * read, which could be zero if we're at the end of the file.
   */
    n = read(fd, buf, size);

    /*
   * We're done with the file for now. Tell UNIX to free the
   * file descriptor. If we don't call close(), we'll run out
   * of file descriptors after a while -- UNIX only lets us
   * have a few dozen (or maybe a few hundred).
   */
    close(fd);

    return (n);
}