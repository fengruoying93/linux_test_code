/* 
 *   backtrace.c 
 */  
#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <signal.h>       /* for signal */  
#include <execinfo.h>     /* for backtrace() */  

extern void signal_handler(int signo);  

void test_func()
{
	char *p = NULL;
	printf("test_func begin.\n");
	*p = 1;
	printf("test_func end.\n");
} 
  
int main(int argc, char *argv[])  
{  
    signal(SIGSEGV, signal_handler);  /* 为SIGSEGV信号安装新的处理函数 */ 	
    test_func();
    return 0;  
}
