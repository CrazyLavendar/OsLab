#include <stdio.h>

int main(int argc, char *argv[], char *env[])
{
    printf("Argument count is %d\n", argc);

    for (int i = 1; i < argc; i++)
    {
        printf("%d Argument value %s\n", i, env[i]);
    }
    return 0;
}