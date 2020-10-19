#include <stdio.h>

int main(int argc, char *argv[], char *env[])
{

    printf("Arg c%d", argc);

    for (int i = 0; i < argc; i++)
    {
        printf("ith Argment %s\n", argv[i]);
    }
    return 0;
}
