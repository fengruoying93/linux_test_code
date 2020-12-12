#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>
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

static char g_localIp[16] = "0.0.0.0";
char *server_file = "server.sock";

int serverProcessClientRequest(int sockfd, char *recv_buf, int recv_len, void *addr)
{
	CMD_TOOL_DATA *cmd_data = (CMD_TOOL_DATA *)recv_buf;
	#ifndef USE_UNIX_DOMAIN_SOCKET
	struct sockaddr_in *cliaddr = (struct sockaddr_in *)addr;
	#else
	struct sockaddr_un *cliaddr = (struct sockaddr_un *)addr;
	#endif

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
	#ifndef USE_UNIX_DOMAIN_SOCKET
    struct sockaddr_in servaddr, cliaddr;
	#else
	struct sockaddr_un servaddr, cliaddr;
	#endif
	socklen_t cliaddr_len;
    int sockfd;
    char recv_buf[512];
	int recv_len;
	char str[16];

	#ifndef USE_UNIX_DOMAIN_SOCKET
	
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
    {
        printf("<%s, %d>socket err,%d:%s\n", __FILE__, __LINE__, errno, strerror(errno));
        return NULL;
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(CMD_TOOL_PORT);
	
	#else

	sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        printf("<%s, %d>socket err,%d:%s\n", __FILE__, __LINE__, errno, strerror(errno));
        return NULL;
    }
	memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sun_family = AF_UNIX;
    strcpy(servaddr.sun_path, server_file);
    if (access(servaddr.sun_path, 0) != -1)
    {
        remove(servaddr.sun_path);
    }
	#endif

    if(bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
		printf("<%s, %d>bind err,%d:%s\n", __FILE__, __LINE__, errno, strerror(errno));
		return NULL;
	}

	printf("<%s, %d>toolServerTask start.\n", __FILE__, __LINE__);
    while (1) 
	{
		cliaddr_len = sizeof(cliaddr);
		bzero(recv_buf, sizeof(recv_buf));
        recv_len = recvfrom(sockfd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr*)&cliaddr, &cliaddr_len);
        //printf("recev from %s at port %d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
		serverProcessClientRequest(sockfd, recv_buf, recv_len, (void*)&cliaddr);
    }
    close(sockfd);
    return arg;
}

