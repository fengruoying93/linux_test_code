#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>

#define CMD_TOOL_PORT 8000

typedef struct _CMD_TOOL_DATA_
{
	int argc;
	char argv[4][16];
}CMD_TOOL_DATA;

static char g_localIp[16] = "0.0.0.0";

int serverProcessClientRequest(int sockfd, char *recv_buf, int recv_len, struct sockaddr_in *cliaddr)
{
	CMD_TOOL_DATA *cmd_data = (CMD_TOOL_DATA *)recv_buf;

	if(!recv_buf || !cliaddr)
	{
		printf("<%s, %d>param error!\n", __FILE__, __LINE__);
		return -1;
	}

	if(!strcmp("getSoftInfo", cmd_data->argv[0]))
	{
		printf("V1.0.0_build201127\n", __FILE__, __LINE__);
	}
	else if(!strcmp("getIp", cmd_data->argv[0]))
	{
		printf("%s\n", g_localIp);
	}
	else if(!strcmp("setIp", cmd_data->argv[0]))
	{
		bzero(g_localIp, sizeof(g_localIp));
		if(0 != cmd_data->argv[1][0])
		{
			snprintf(g_localIp, sizeof(g_localIp), "%s", cmd_data->argv[1]);
		}
	}
	
	return 0;
}

void *toolServerTask(void *arg)
{
    struct sockaddr_in servaddr, cliaddr;
	socklen_t cliaddr_len;
    int sockfd;
    char recv_buf[512];
	int recv_len;
	char str[16];
 
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(CMD_TOOL_PORT);
 
    bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	printf("<%s, %d>toolServerTask start.\n", __FILE__, __LINE__);
    while (1) 
	{
		cliaddr_len = sizeof(cliaddr);
		bzero(recv_buf, sizeof(recv_buf));
        recv_len = recvfrom(sockfd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr*)&cliaddr, &cliaddr_len);
        //printf("recev from %s at port %d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
		serverProcessClientRequest(sockfd, recv_buf, recv_len, &cliaddr);
    }
    close(sockfd);
    return arg;
}

