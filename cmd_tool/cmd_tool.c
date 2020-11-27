#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define CMD_TOOL_PORT 8000

typedef struct _CMD_TOOL_DATA_
{
	int argc;
	char argv[4][16];
}CMD_TOOL_DATA;


int main(int argc, char *argv[])
{
	struct sockaddr_in servaddr;
	int sockfd;
	CMD_TOOL_DATA cmd;
	int i = 0;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0); 

	bzero(&servaddr, sizeof(servaddr)); 
	servaddr.sin_family = AF_INET; 
	inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
	servaddr.sin_port = htons(CMD_TOOL_PORT);

	memset(&cmd, 0, sizeof(cmd));
	cmd.argc = argc;
	for(i = 0; i < argc && i < 4; i++){
		snprintf(cmd.argv[i], 16, "%s", argv[i]);
	}

	if(-1 == sendto(sockfd, &cmd, sizeof(cmd), 0, (struct sockaddr*)&servaddr, sizeof(struct sockaddr_in)))
	{
		printf("<%s, %d>sendto error!\n", __FILE__, __LINE__);
		return -1;
	}
	
	return 0;
}

