#include <stdio.h>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/shm.h>

typedef struct{
    int LoaderInput;
    int BinaryInput;
    int BinTerminated;
}exchange_t;

int num = 40;

int add5(int input)
{
    return input + 5;
}

int main(int argc, char **argv, char **envp)
{
      // ftok to generate unique key
    key_t key = ftok("shmfile",65);
  
    // shmget returns an identifier in shmid
    int shmid = shmget(key,1024,0666|IPC_CREAT);
  
    // shmat to attach to shared memory
    exchange_t *exchange_struct = (exchange_t*) shmat(shmid,(void*)0,0);
  
    exchange_struct->BinTerminated = 0;
      

    printf("Hello, World!: from %ld\n", (long)getpid());
    for (int i = 0; i < 10; ++i)
    {
    sleep(1);
    //printf("%d\n", num);
    printf("Child process input: %d\n",exchange_struct->BinaryInput);

    exchange_struct->LoaderInput += 1;
    }
    printf("Child terminates: %ld\n", (long)getpid()); 

    exchange_struct->BinTerminated = 1;

        //detach from shared memory 
    shmdt(exchange_struct);
    


    return 0;
}