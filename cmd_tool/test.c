#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

extern void *toolServerTask(void *arg);

int main(int argc, char *argv[])
{
	pthread_t ntid;
	
	if (0 != pthread_create(&ntid, NULL, toolServerTask, NULL))
	{
		printf("<%s, %d>create toolServerTask fail!\n", __FILE__, __LINE__);
	}
	
	while(1)
	{
		pause();
	}
	
	return 0;
}

