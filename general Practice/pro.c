#include<stdio.h>
int main()
{
int p;
printf(“ Origin Program, pid = %d\n ” , getpid());
p=fork();
if(p==0)
{
printf(“ In child process pid = %d , ppid= %d “, getpid(),getppid());
}
else
{
printf(“ In parent pid = %d, fork returned = %d \n”, getpid(),p);
}
}