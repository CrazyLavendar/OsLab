//implementing ls command (provided -l option)

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>

int main(int argc, char *argv[])
{
    DIR *dirp;
    struct dirent *entp;
    struct stat s;
    struct passwd *p;
    struct group *g;
    time_t t;
    int i = 1, l = 0, j, longlst = 0, argn = 0;
    char type;

    for (j = 1; j < argc; j++)
    {
        if (argv[j][0] == '-' && argv[j][1] == 'l')
            longlst = 1;
        else
            argn++;
    }
    if (argc > 1 && argv[i][0] == '-' && argv[i][1] == 'l')
        i++;

    if (argv[i] == NULL || (argn == 0 && longlst == 1))
    {
        dirp = opendir(".");
        if (argn > 1)
            printf("\n\n. :\n");
    }
    else
    label:
    {
        dirp = opendir(argv[i]);
        if (argn > 1)
            printf("\n\n%s :\n", argv[i]);
        i++;
    }

        if (dirp == NULL)
        {
            printf("error opening directory");
            if (argv[i] != NULL)
            {
                goto label;
            }
            else
            {
                exit(0);
            }
        }

    if (longlst == 0)
    {
        while ((entp = readdir(dirp)) != NULL)
        {
            if (entp->d_name[0] == '.')
                continue;
            else
                printf("%s\t", entp->d_name);
        }
    }
    else
    {
        while ((entp = readdir(dirp)) != NULL)
        {
            if (entp->d_name[0] == '.')
                continue;
            else
            {

                int k = stat(entp->d_name, &s);
                if (k == -1)
                {
                    printf("%s\t", entp->d_name);
                    printf("STAT ERROR\n");
                }
                else
                {

                    if (S_ISLNK(s.st_mode))
                        type = 'l'; /*symlink*/
                    else if (S_ISDIR(s.st_mode))
                        type = 'd'; /*directory*/
                    else if (S_ISCHR(s.st_mode))
                        type = 'c'; /*character raw device*/
                    else if (S_ISBLK(s.st_mode))
                        type = 'b'; /*block raw device*/
                    else if (S_ISFIFO(s.st_mode))
                        type = 'p'; /*named pipe*/
                    else if (S_ISSOCK(s.st_mode))
                        type = 's'; /*Unix domain socket*/
                    else if (S_ISREG(s.st_mode))
                        type = '-'; /*regular file*/
                    else
                        type = '?';

                    printf("%c", type);
                    printf("%c", (s.st_mode & 00400) ? 'r' : '-');
                    printf("%c", (s.st_mode & 00200) ? 'w' : '-');
                    printf("%c", (s.st_mode & 00100) ? 'x' : '-');
                    printf("%c", (s.st_mode & 00040) ? 'r' : '-');
                    printf("%c", (s.st_mode & 00020) ? 'w' : '-');
                    printf("%c", (s.st_mode & 00010) ? 'x' : '-');
                    printf("%c", (s.st_mode & 00004) ? 'r' : '-');
                    printf("%c", (s.st_mode & 00002) ? 'w' : '-');
                    printf("%c", (s.st_mode & 00001) ? 'x' : '-');

                    printf("  %-4d ", (int)s.st_nlink);

                    p = getpwuid(s.st_uid);
                    printf("   %s", p->pw_name);

                    g = getgrgid(s.st_gid);
                    printf("   %s", g->gr_name);

                    printf("%7d ", (int)s.st_size);
                    printf("%-18s", entp->d_name);
                    t = s.st_mtime;
                    printf("%s", ctime(&t));
                }
            }
        }
    }
    closedir(dirp);

    if (argv[i] != NULL)
    {
        if (argv[i][0] == '-' && argv[i][1] == 'l')
            i++;
        if (argv[i] != NULL)
            goto label;
    }

    printf("\n");
    return (0);
}