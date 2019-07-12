#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
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
    int rdData;
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
	shmaddr = shared_Mem;

    fd1 = open("copied", O_WRONLY|O_CREAT|O_TRUNC, 0666);
    signal(SIGCONT, sigHandler1);
    signal(SIGCHLD, sigHandler2);
    kill(atoi(argv[1]), SIGUSR1);

    while(1)
    {
        pause();
        kill(atoi(argv[1]), SIGUSR1);
    }

	//sleep(4);
	
	// step4. shmdt
	if (shmdt(shared_Mem) == -1)
	{
		fprintf(stderr, "shmdt failed\n");
		exit(EXIT_FAILURE);
	}
	
	// step5. shmctl : IPC_RMID
	if (shmctl(shmid, IPC_RMID, 0) == -1)
	{
		fprintf(stderr, "shmctl (IPC_RMID) failed\n");
		exit(EXIT_FAILURE);
	}
	
	exit(EXIT_SUCCESS);
}
void sigHandler1(int sig)
{
    int len;
    if((len = write(fd1, shmaddr, strlen(shmaddr))) != 0)
       ; 
    else
        exit(EXIT_SUCCESS);
}
void sigHandler2(int sig)
{
    close(fd1);
    exit(EXIT_SUCCESS);
}
