#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

extern "C"{
extern void show_mem_info();
}

void* thread_func1(void *arg)
{
    char *p = NULL;
	while(1)
	{
		p = (char *)malloc(5);
		memset(p, 1, 5);
		free(p);
		sleep(3);
	}
    return arg;
}

void* thread_func2(void *arg)
{
    char *p = NULL;
	while(1)
	{
		p = (char *)malloc(10);
		memset(p, 2, 10);
		//free(p);
		sleep(5);
	}
    return arg;
}

int main(int argc, char *argv[])
{
	pid_t pid;
    pthread_t tid;
	
    pid = getpid();
    printf("process id=%d\n", pid);
    if(0 != pthread_create(&tid, NULL, thread_func1, NULL))
    {
		printf("pthread_create thread_func1 fail.\n");
	}
	if(0 != pthread_create(&tid, NULL, thread_func2, NULL))
    {
		printf("pthread_create thread_func2 fail.\n");
	}
	
	while(1)
	{
		show_mem_info();
		sleep(5);
	}
	return 0;
}
