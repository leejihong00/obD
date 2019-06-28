#include <stdio.h>
#include <sys/time.h>

int main(void)
{
    long long start=1, end=10000000, result=0;
    struct timeval time1, time2, time3;

    gettimeofday(&time1, NULL);
    
    for(;start<=end;start++)
        result += start;

    gettimeofday(&time2, NULL);

    time1.tv_sec %= 1000000;
    time1.tv_sec *= 1000000;
    time2.tv_sec %= 1000000;
    time2.tv_sec *= 1000000;
    time3.tv_sec = time2.tv_sec+time2.tv_usec - 
                    (time1.tv_sec+time1.tv_usec);
    printf("%ld sec\n",time3.tv_sec);
}
