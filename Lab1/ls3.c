#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <fcntl.h>

char buffer[512];
void printls(int len, struct dirent **rdir, int a, int l, int d);
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
    DIR *dirp;            // Open directory pointer
    struct dirent **rdir; // Directory entry pointer
    int i, j, count = 0;
    int l = 0, a = 0, d = 0; // -l, -a, -d switches
    char file[256], tem[20], stfile[4096], dirfile[4096];
    char username[20], groupname[20];
    struct stat buf;   // Stat buffer - attributes of a file
    struct passwd *pw; // Password structure
    struct group *gw;  // Group structure
    int len;
    time_t time;
    int chars = 0;

    // Default directory - current directory

    // Get the current working directory
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
        else // If else doesn't executes , This program does ls for current working directory
        {
            // Assuming that only one last arguent can be
            // given as directory name for listing
            // closedir(dirp);
            // dirp = opendir(argv[i]);
            strcpy(dirfile, argv[i]);

            break; // Why breaking? Because - Accepting only one directory name
        }
    }

    len = scandir(dirfile, &rdir, NULL, alphasort);

    //printls(len, rdir, a, l, d);

    if (len < 0)
    {
        printf("No such directory present : %s\n", dirfile);
    }

    // You can take x is a Id of a file prsent in the working directory
    for (int x = 0; x < len; x++)
    {
        strcpy(file, rdir[x]->d_name);

        //Default
        if ((a == 0) && (file[0] != '.') && l == 0)
            printf("%s\t", rdir[x]->d_name);

        else if ((a == 1) && l == 0)
            printf("%s\t", rdir[x]->d_name);

        else if ((l == 1))
        {
            if ((a == 0) && (file[0] == '.')) //If a switch is 0 , It means skipping for hidden files
                continue;

            stat(rdir[x]->d_name, &buf);
            tomode(&buf, tem);

            printf("%.10s%4ld ", tem, buf.st_nlink);

            // Get uid==> user name,
            pw = getpwuid(buf.st_uid);
            printf("%10s ", pw->pw_name);

            // gid==>group name
            gw = getgrgid(buf.st_gid);
            printf("%10s ", gw->gr_name);

            /* Actual ls command prints no# of chars in each files as well. 
                    // Removing this multiple line comment can print # of chars in each file.. 
                    // But it takes huge time if the directory contains too many files
                    int fd;
                    char ch;

                    fd = open(rdir[x]->d_name, O_RDONLY, 0);

                    while ((count = read(fd, &ch, 1)) > 0)
                    {
                        chars++;
                    }

                    close(fd);

                    printf("%d ", chars);
                    chars = 0;
                    */

            // Printing time
            time = buf.st_atime;
            printf("%.12s ", ctime(&time) + 4); // Omitting extras. Printing similar to ls -l time

            //Finally printing file name
            printf("%s\n", rdir[x]->d_name);
        }
    }

    return 0;
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

// Iam not using this
char *chartime(time_t secs, char str[25])
{
    struct tm dt;
    /////////////

    sprintf(str, "%04d-%02d-%02d",
            dt.tm_year + 1900,
            dt.tm_mon + 1,
            dt.tm_mday);

    str = "vimal";
    return str;
}

//Sort the output based on name, while displaying.
//Sort the output based on last modification date.
//Add support for -R option.
//Add support for -r option.
//Read the manual pages of other directory related system calls: telldir, seekdir, scandir.