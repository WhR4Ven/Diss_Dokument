#include <stdio.h>
#include <unistd.h>

int num = 40;

int add5(int input)
{
    return input + 5;
}

int main(int argc, char **argv, char **envp)
{
    int* pipefd = (int*)envp;
    char    readbuffer[80];
    int nbytes;
    int* pipe_ptr;

    pipe_ptr = (int*)*envp;


    printf("Hello, World!: from %ld\n", (long)getpid());
    for (int i = 0; i < 10; ++i)
    {
    num = add5(num);
    sleep(1);
    printf("%d\n", num);
    nbytes = read(*pipe_ptr, readbuffer, sizeof(readbuffer));
    printf("Read string: %s\n", readbuffer);
    }
    printf("Child terminates: %ld\n", (long)getpid()); 
    return 0;
}