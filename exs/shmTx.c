#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

//For IPC 
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHMSIZE	4096

void sigHandler1(int sig);
void sigHandler2(int sig);

int fd1;
char *shmaddr;

int main(int argc, char * argv[])
{
    void *shared_Mem = (void*)0;
    int shmid;
    int i;

    // step0. ftok()

    // step1. shmget
    shmid = shmget((key_t)1234, SHMSIZE, 0666 | IPC_CREAT);
    if (shmid == -1)
    {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }

    // step2. shmat
    shared_Mem = shmat(shmid, (void*)0, 0);
    if (shared_Mem == (void*)-1)
    {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }

    printf("Memory attached at %X\n", (int)shared_Mem);
    shmaddr = (char*)shared_Mem;

    if((fd1 = open(argv[1], O_RDONLY)) == -1)
    {
        perror("open()");
        exit(EXIT_FAILURE);
    }
    signal(SIGUSR1, sigHandler1);
    signal(SIGUSR2, sigHandler2);
    while(1)
    {
        continue;
    }

    //sleep(4);

    // step4. shmdt
    if (shmdt(shared_Mem) == -1)
    {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }
    /*
    // step5. shmctl : IPC_RMID
    if (shmctl(shmid, IPC_RMID, 0) == -1)
    {
    fprintf(stderr, "shmctl (IPC_RMID) failed\n");
    exit(EXIT_FAILURE);
    }
    */
    exit(EXIT_SUCCESS);
}
void sigHandler1(int sig)
{
    memset(shmaddr, '\0', SHMSIZE);
    if(read(fd1, shmaddr, SHMSIZE) > 0)
    {
        printf("shmTx>> sent\n");
        kill(-1, SIGCONT);
    }
    else
    {
        printf("shmTx>> finished\n");
        kill(-1, SIGCHLD);
        exit(EXIT_SUCCESS);
    }
    
}
void sigHandler2(int sig)
{
    printf("shmTx>> SIGUSR2 received\n");
    exit(EXIT_SUCCESS);
}
