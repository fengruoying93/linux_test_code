#include <iostream>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

typedef struct
{
	void *addr;
	unsigned int size;
	unsigned int tid;
}MEM_INFO;

#define CHUNK_MAX_LEN  1024*1024

map<void *, MEM_INFO> malloc_map;
unsigned int chunk_map[CHUNK_MAX_LEN];  //记录1MB以内的内存分配个数

extern "C"{

void* __real_malloc(size_t size); // 只声明不定义__real_malloc
void __real_free(void *ptr);

void* __wrap_malloc(size_t size) // 定义__wrap_malloc
{
	void *addr = NULL;
	unsigned int *malloc_size = NULL;
	
    //printf("__wrap_malloc, size:%u\n", size);
    addr = __real_malloc(size+8); // 通过__real_malloc调用真正的malloc
    if(addr)
    {
		memset(addr, 0, 8);  //多分配的前8字节用于存储该chunk的size
		malloc_size = (unsigned int*)addr;
		*malloc_size = size;
		++chunk_map[size];

		//插入map表记录
		MEM_INFO node;
		node.addr = addr+8;
		node.size = size;
		node.tid = pthread_self();
		malloc_map[node.addr] = node;
		
		return (addr+8);
	}
	return NULL;
}

void __wrap_free(void *ptr)
{
    //printf("__wrap_free, ptr:0x%u\n", ptr);

	unsigned int *malloc_size = (unsigned int*)(ptr-8);
	--chunk_map[*malloc_size];
	
    __real_free(ptr-8);

	//删除map表记录
	map<void *, MEM_INFO>::iterator it = malloc_map.find(ptr);
	if(it != malloc_map.end())
	{
		malloc_map.erase(it);
	}
}

void show_mem_info()
{
	int i = 0;
	map<void *, MEM_INFO>::iterator it;
	
	for(it = malloc_map.begin(); it != malloc_map.end(); it++)
	{
		printf("[memInfo %d]0x%x, %u, %u\n", i+1, it->second.addr, it->second.size, it->second.tid);
		i++;
	}
	/*for(i = 0; i < CHUNK_MAX_LEN; i++)
	{
		printf("%u-%u ", i, chunk_map[i]);
	}*/
	printf("\n");
}

}
