#include <sys/syscall.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <err.h>

/* for errno */
#include <errno.h>

int main(int argc, char **argv, char **envp)
{
    int     filedes[2], nbytes;
    FILE * pFile;
    long size;
    int fd;
    unsigned char *buffer; //buffer

    printf("Hello, I'm your host: %ld\n", (long)getpid());  ;

    pFile = fopen ("bin.o" , "r");

    fseek(pFile, 0L, SEEK_END);
    size = ftell(pFile);
    rewind (pFile);

    printf("elf size %ld\n", size);
    buffer = (unsigned char *) malloc(size);
    fread(buffer, sizeof *buffer, size, pFile);
    fclose (pFile);

    fd = syscall (SYS_memfd_create, "name", 0);

    int bytes = write(fd, buffer, size);
    if (bytes == -1)
    {
        printf("write failed");
        exit(errno);
    }
    else
    {
        printf("bytes written %d\n", bytes);
    }
    free(buffer);

    pipe(filedes); // create the pipe
    int* pipe_ptr = &filedes[0];


    pid_t pid = fork(); //fork process
    printf("pid %d\n", pid);

    if (pid == 0) { //child
    char    readbuffer[80];

    const char * const argv_c[] = {"script", NULL};
    const char * const envp_c[] = {(char*)pipe_ptr, NULL};
    pipe_ptr = (int*)*envp_c;

    nbytes = read(*pipe_ptr, readbuffer, sizeof(readbuffer));
    printf("Read string_c: %s\n", readbuffer);

    if (fexecve(fd, (char * const *) argv_c, (char * const *) envp_c) == -1)
         exit(errno);
    }
    else if (pid == -1)
        err(1, "%s failed", "fork");
    else    //parent
    {
        char    string[] = "Hello, world!\n";
        printf("host write pipe\n");
        write(filedes[1], string, (strlen(string)+1));
        for(int i = 0; i < 10; ++i)
        {
        //printf("parent process\n");
        sleep(1);
        }
    }

    printf("Host terminates: %ld\n", (long)getpid());  ;
}