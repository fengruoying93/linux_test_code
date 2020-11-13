#include <iostream>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <execinfo.h>     /* for backtrace() */

using namespace std;

typedef struct
{
	void *addr;
	unsigned int size;
	unsigned int tid;
}MEM_INFO;

#define CHUNK_MAX_LEN  1024*1024
#define BACKTRACE_SIZE   16
#define MEMORY_LEAK_WARNING_LIMIT  10

map<void *, MEM_INFO> malloc_map;  //��¼��ǰ���е�malloc�ڴ���Ϣ
unsigned int chunk_map[CHUNK_MAX_LEN];  //��¼1MB���ڵ��ڴ�������
pthread_mutex_t mem_mutex = PTHREAD_MUTEX_INITIALIZER;

unsigned int chunk_max_num = 0;  //����������chunk������
unsigned int chunk_max_num_size = 0; //����������chunk��size

extern "C"{

void* __real_malloc(size_t size); // ֻ����������__real_malloc
void __real_free(void *ptr);


void dump(void)  
{  
    int j, nptrs;  
    void *buffer[BACKTRACE_SIZE];  
    char **strings;  
	
    nptrs = backtrace(buffer, BACKTRACE_SIZE);  
	
    printf("backtrace returned %d addresses\n", nptrs);  
  
    /*strings = backtrace_symbols(buffer, nptrs);  
    if (strings == NULL) {  
        perror("backtrace_symbols");  
        exit(EXIT_FAILURE);  
    }*/ 
  
    for (j = 0; j < nptrs; j++)  
		printf("[backtrace]%d-0x%x\n", j, buffer[j]);  
    //free(strings);
}  


void* __wrap_malloc(size_t size) // ����__wrap_malloc
{
	void *addr = NULL;
	unsigned int *chunk_size = NULL;
	
    //printf("__wrap_malloc, size:%u\n", size);
    addr = __real_malloc(size+8); // ͨ��__real_malloc����������malloc
	
    if(addr)
    {
		memset(addr, 0, 8);  //������ǰ8�ֽ����ڴ洢��chunk��size
		chunk_size = (unsigned int*)addr;
		*chunk_size = size;

		//����map���¼
		MEM_INFO node;
		node.addr = addr+8;
		node.size = size;
		node.tid = pthread_self();
		
		pthread_mutex_lock(&mem_mutex);
		malloc_map[node.addr] = node;
		++chunk_map[size];
		pthread_mutex_unlock(&mem_mutex);

		if(size == chunk_max_num_size && chunk_max_num > MEMORY_LEAK_WARNING_LIMIT)
		{
			dump();
		}
		return (addr+8);
	}
	return NULL;
}

void __wrap_free(void *ptr)
{
    //printf("__wrap_free, ptr:0x%u\n", ptr);

	unsigned int *chunk_size = (unsigned int*)(ptr-8);

	//ɾ��map���¼
	pthread_mutex_lock(&mem_mutex);
	map<void *, MEM_INFO>::iterator it = malloc_map.find(ptr);
	if(it != malloc_map.end())
	{
		//--chunk_map[it->second.size];
		malloc_map.erase(it);
	}
	--chunk_map[*chunk_size];
	pthread_mutex_unlock(&mem_mutex);

	__real_free(ptr-8);
}

void show_mem_info()
{
	int i = 0;
	map<void *, MEM_INFO>::iterator it;
	unsigned int tmp_max_num = 0;
	unsigned int tmp_max_num_size = 0;

	pthread_mutex_lock(&mem_mutex);
	for(i = 0; i < CHUNK_MAX_LEN; i++)
	{
		//printf("%u-%u ", i, chunk_map[i]);
		if(chunk_map[i] > tmp_max_num)
		{
			tmp_max_num = chunk_map[i];
			tmp_max_num_size = i;
		}
	}
	chunk_max_num = tmp_max_num;
	chunk_max_num_size = tmp_max_num_size;
	printf("max_num_size:%u, max_num:%u\n", chunk_max_num_size, chunk_max_num);
	
	i = 0;
	for(it = malloc_map.begin(); it != malloc_map.end(); it++)
	{
		printf("[mallocMap %d]addr:0x%x, size:%u, tid:%u\n", i+1, it->second.addr, it->second.size, it->second.tid);
		i++;
	}
	pthread_mutex_unlock(&mem_mutex);
	
	printf("\n");
}

}
