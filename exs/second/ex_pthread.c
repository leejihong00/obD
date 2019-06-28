#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

int glob_var = 6;

void *t_function(void *data)
{
    int id;
    int i=0;
    pthread_t t_id;
    id = *((int*)data);
    t_id = pthread_self();
    glob_var++;
    printf("pid %d t_id %lu id %d glob_var %d\n", getpid(), t_id, id, glob_var);
    sleep(2);
    return (void*)(id*id);
}

int main(void)
{
    pthread_t p_thread[2];
    int status;
    int a=1, b=2;

    printf("befor pthread_create() \npid %d glob_var %d\n", 
            getpid(), glob_var);

    //creating thread 1
    if(pthread_create(p_thread, NULL, t_function, (void*)&a) < 0)
    {
        perror("pthread_create()");
        exit(1);
    }
    //creating thread 2
    if(pthread_create(p_thread+1, NULL, t_function, (void*)&b) < 0)
    {
        perror("pthread_create()");
        exit(2);
    }

    pthread_join(p_thread[0], (void**)&status);
    printf("thread_join %d\n", status);
    pthread_join(p_thread[1], (void**)&status);
    printf("thread_join %d\n", status);
    
    printf("after pthread_create() \npid %d glob_var %d\n", 
            getpid(), glob_var);
}
