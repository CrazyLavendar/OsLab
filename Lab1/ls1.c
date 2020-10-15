/////////////////////////////////////////////////////////////////////
// UNIX Commands prototype implementation
// Command: ls
// Options supported: -l -a [<dirname>]
// Authors: Dr Chapram Sudhakar, Dr Rashmi Ranjan Rout
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>

void tomode(struct stat *buf, char *str);
int ls(int argc, char *argv[]);
char *chartime(time_t secs, char str[25]);

char buffer[512];

int main(int argc, char *argv[])
{
    return ls(argc, argv);
}

int ls(int argc, char *argv[])
{
    DIR *dirp;           // Open directory pointer
    struct dirent *rdir; // Directory entry pointer
    int i, j, count = 0;
    int l = 0, a = 0, d = 0; // -l, -a, -d switches
    char file[256], tem[20], stfile[4096], dirfile[4096];
    char username[20], groupname[20];
    struct stat buf;   // Stat buffer - attributes of a file
    struct passwd *pw; // Password structure
    struct group *gw;  // Group structure
    int len;

    // Default directory - current directory

    // Get the current working directory
    getcwd(dirfile, 4096);

    for (i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            for (j = 1; j < strlen(argv[i]); j++)
            {
                switch (argv[i][j])
                {
                case 'l':
                    l = 1;
                    break;
                case 'a':
                    a = 1;
                    break;
                case 'd':
                    d = 1;
                    break;
                default:
                    break;
                }
            }
        }
        else
        {
            // Assuming that only one last arguent can be
            // given as directory name for listing
            closedir(dirp);
            dirp = opendir(argv[i]);
            strcpy(dirfile, argv[i]);
        }
    }

    if (dirp == NULL)
    {
        sprintf(buffer, "Error: directory %s opening ", dirfile);
        perror(buffer);
        return 0;
    }

    while (1)
    {
        rdir = readdir(dirp);

        if (rdir == NULL)
        {
            // No more entries
        }
        strcpy(file, rdir->d_name);

        // No -a argument and hidden file, so skip this entry
        if ((a == 0) && (file[0] == '.'))
            continue;

        if (l == 1)
        {
            // Get attributes, first construct complete path

            // Get status (attributes) of the entry

            // Converting mode byte into human
            // readable rwx triplet format
            tomode(&buf, tem);

            printf("%s%4ld ", tem, buf.st_nlink);

            // Get uid==> user name,
            pw = getpwuid(buf.st_uid);
            printf("%10s", pw->pw_name);

            // gid==>group name
            gw = getgrgid(buf.st_gid);
            printf("%10s", gw->gr_name);

            // Converting to human readable
            // time-date format
        }
        else
        {
            // Display only the name of the file
            // Assuming smaller names. Add support for
            // displaying arbitrary longer names
        }
    }
}

void tomode(struct stat *buf, char *tem)
{

    switch (buf->st_mode & S_IFMT)
    {
    case S_IFDIR:
        tem[0] = 'd';
        break;
    case S_IFCHR:
        tem[0] = 'c';
        break;
    case S_IFBLK:
        tem[0] = 'b';
        break;
    case S_IFIFO:
        tem[0] = 'p';
        break;
    case S_IFLNK:
        tem[0] = 'l';
        break;
    default:
        tem[0] = '-';
        break;
    }

    if (buf->st_mode & S_IRUSR)
        tem[1] = 'r';
    else
        tem[1] = '-';
    if (buf->st_mode & S_IWUSR)
        tem[2] = 'w';
    else
        tem[2] = '-';
    if (buf->st_mode & S_IXUSR)
        tem[3] = 'x';
    else
        tem[3] = '-';

    if (buf->st_mode & S_IRGRP)
        tem[4] = 'r';
    else
        tem[4] = '-';
    if (buf->st_mode & S_IWGRP)
        tem[5] = 'w';
    else
        tem[5] = '-';
    if (buf->st_mode & S_IXGRP)
        tem[6] = 'x';
    else
        tem[6] = '-';

    if (buf->st_mode & S_IROTH)
        tem[7] = 'r';
    else
        tem[7] = '-';
    if (buf->st_mode & S_IWOTH)
        tem[8] = 'w';
    else
        tem[8] = '-';
    if (buf->st_mode & S_IXOTH)
        tem[9] = 'x';
    else
        tem[9] = '-';

    return;
}

char *chartime(time_t secs, char str[25])
{
    struct tm dt;
    /////////////

    return str;
}

//Sort the output based on name, while displaying.
//Sort the output based on last modification date.
//Add support for -R option.
//Add support for -r option.
//Read the manual pages of other directory related system calls: telldir, seekdir, scandir.