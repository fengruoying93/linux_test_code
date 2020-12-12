#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

#define USE_UNIX_DOMAIN_SOCKET

#define CMD_TOOL_PORT 8000

typedef struct _CMD_TOOL_DATA_
{
	int argc;
	char argv[4][16];
}CMD_TOOL_DATA;

char* server_file = "server.sock";
char* client_file = "client.sock";

int main(int argc, char *argv[])
{
	#ifndef USE_UNIX_DOMAIN_SOCKET
	struct sockaddr_in servaddr;
	#else
	struct sockaddr_un servaddr, cliaddr;
	#endif
	int sockfd;
	CMD_TOOL_DATA cmd;
	int i = 0;

	#ifndef USE_UNIX_DOMAIN_SOCKET

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
    {
        printf("<%s, %d>socket err,%d:%s\n", __FILE__, __LINE__, errno, strerror(errno));
        return -1;
    }
	bzero(&servaddr, sizeof(servaddr)); 
	servaddr.sin_family = AF_INET; 
	inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
	servaddr.sin_port = htons(CMD_TOOL_PORT);
	
	#else

	sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        printf("<%s, %d>socket err,%d:%s\n", __FILE__, __LINE__, errno, strerror(errno));
        return -1;
    }
	memset(&cliaddr, 0, sizeof(cliaddr));
    cliaddr.sun_family = AF_UNIX;
    strcpy(cliaddr.sun_path, client_file);
    if (access(cliaddr.sun_path, 0) != -1)
    {
        remove(cliaddr.sun_path);
    }
	if(bind(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr)) < 0) //unix domain socket必须由bind()创建
    {
		printf("<%s, %d>bind err,%d:%s\n", __FILE__, __LINE__, errno, strerror(errno));
		return -1;
	}
	memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sun_family = AF_UNIX;
    strcpy(servaddr.sun_path, server_file);
	#endif

	memset(&cmd, 0, sizeof(cmd));
	cmd.argc = argc;
	for(i = 0; i < argc && i < 4; i++){
		snprintf(cmd.argv[i], 16, "%s", argv[i]);
	}

	if(-1 == sendto(sockfd, &cmd, sizeof(cmd), 0, (struct sockaddr*)&servaddr, sizeof(servaddr)))
	{
		printf("<%s, %d>sendto error!\n", __FILE__, __LINE__);
		return -1;
	}
	
	return 0;
}

