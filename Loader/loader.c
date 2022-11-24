#include <sys/syscall.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <err.h>

#include <sys/ipc.h>
#include <sys/shm.h>

/* for errno */
#include <errno.h>


typedef struct{
    int LoaderInput;
    int BinaryInput;
    int BinTerminated;
}exchange_t;

int main(int argc, char **argv, char **envp)
{
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

    const char * const argv_c[] = {"script", NULL};
    const char * const envp_c[] = {"", NULL};


    pid_t pid = fork(); //fork process

    printf("pid %d\n", pid);

    if (pid == 0) { //child

    if (fexecve(fd, (char * const *) argv_c, (char * const *) envp_c) == -1)
         exit(errno);
    }
    else if (pid == -1)
        err(1, "%s failed", "fork");
    else    //parent
    {
        int num = 1;

        char    string[] = "pipe write host\n";
         // ftok to generate unique key
        key_t key = ftok("shmfile2",65);
    
        // shmget returns an identifier in shmid
        int shmid = shmget(key,1024,0666|IPC_CREAT);
    
        // shmat to attach to shared memory
        exchange_t *exchange_struct = (exchange_t*) shmat(shmid,(void*)0,0);

        while(exchange_struct->BinTerminated == 0)
        {
            exchange_struct->BinaryInput += 1;

            printf("parent process input: %d\n", exchange_struct->LoaderInput);


            sleep(1);
        }
    //detach from shared memory 
    shmdt(exchange_struct);

    // destroy the shared memory
    shmctl(shmid,IPC_RMID,NULL);
    }

    printf("Host terminates: %ld\n", (long)getpid());  ;


}