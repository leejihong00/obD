#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h> // getimeofday( ) 함수에서 사용
#include <stdlib.h>

struct value
{
	int start;
	int end;
};


void *add_func(void *data)
{
	int i;
	long long result=0;
	struct value value1;
	pthread_t t_id; value1 = *((struct value *)data);

	//printf("start = %d\n", value1.start);
	//printf("end = %d\n", value1.end);

	for(i=value1.start;i<value1.end+1;i++)
	{
		result += i;
	}
	printf("result = %lld\n", result);
	printf("(void *)result = %p\n",(void *)result);
	return (void *)(result);
}


long long adder(int start, int end)
{
	int i;
	long long result=0;

	for(i = start ;i<=end; i++)
	{
		result += i;
	}

	return result;
}

void disp_runtime(struct timeval UTCtime_s, struct timeval UTCtime_e)
{
	struct timeval UTCtime_r;
	if((UTCtime_e.tv_usec- UTCtime_s.tv_usec)<0)
	{
		UTCtime_r.tv_sec  = UTCtime_e.tv_sec - UTCtime_s.tv_sec - 1;
		UTCtime_r.tv_usec = 1000000 - UTCtime_e.tv_usec - UTCtime_s.tv_usec;
	}
	else
	{
		UTCtime_r.tv_sec = UTCtime_e.tv_sec - UTCtime_s.tv_sec;
		UTCtime_r.tv_usec = UTCtime_e.tv_usec - UTCtime_s.tv_usec;
	}
	printf("runtime : %ld sec %ld\n", UTCtime_r.tv_sec, UTCtime_r.tv_usec);
}


int main(int argc, char *argv[])
{
	pthread_t p_thread[2];
	int thread_num;
	int err;
	long long sum1, sum2;
	long long result;
	int end = atoi(argv[2]);
	struct value value1,value2;
	struct timeval UTCtime_s, UTCtime_e, UTCtime_r;

	thread_num = atoi(argv[1]); 

	switch(thread_num)
	{
		case 1: gettimeofday(&UTCtime_s, NULL);
				result = adder(1, end);
				gettimeofday(&UTCtime_e, NULL);
				disp_runtime(UTCtime_s, UTCtime_e);
				printf("1 thread result = %lld\n", result); 
				break;

		case 2: value1.start = 1;
				value1.end   = end>>1;
				value2.start = (value1.end)+1;
				value2.end   = end;

				gettimeofday(&UTCtime_s, NULL);
    			if((err = pthread_create(&p_thread[0], NULL, add_func, (void*)&value1)) < 0)
			    {
					perror("thread create error : "); 
					exit(1);
				}

				if((err = pthread_create (&p_thread[1], NULL, add_func, (void*)&value2)) < 0)
				{
					perror("thread create error : ");
				    exit(2);
				}

				pthread_join(p_thread[0], (void **)&sum1);
				//printf("add1 result : %lld\n", sum1);
				pthread_join(p_thread[1], (void **)&sum2);
				//printf("add2 result : %lld\n", sum2);
				gettimeofday(&UTCtime_e, NULL);
				result = sum1 + sum2;
				
				disp_runtime(UTCtime_s, UTCtime_e);
				printf("2 thread result = %lld\n", result); 
				break;
		default:
				printf("parameter error\n");
	}

	return 0;
}