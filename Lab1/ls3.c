#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>

char buffer[512];

int ls(int argc, char *argv[]);

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

    // Default directory - current directory

    // Get the current working directory
    getcwd(dirfile, 4096);

    printf("%s\n", dirfile);
    len = scandir(".", &rdir, NULL, alphasort);
    while (len--)
    {
        printf("%s\t", rdir[len]->d_name);
    }
}