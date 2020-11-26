#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>
 
#define SERVER_PORT 8000
#define CLIENT_PORT 9000
#define GROUP "239.0.0.2"
 
int main(int argc, char *argv[])
{
    struct sockaddr_in localaddr;
    socklen_t addr_len = sizeof(localaddr);
    int confd;
    ssize_t len;
    char buf[BUFSIZ];
 
    confd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&localaddr, sizeof(localaddr));                                   /* 初始化*/
    localaddr.sin_family = AF_INET;
    //inet_pton(AF_INET, "0.0.0.0" , &localaddr.sin_addr.s_addr);
    localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localaddr.sin_port = htons(atoi(argv[2]));  //CLIENT_PORT
	
    //int opt = SO_REUSEADDR;
    //setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
 
    bind(confd, (struct sockaddr *)&localaddr, sizeof(localaddr));
 
    #if 0
    struct ip_mreqn group;                                                  /*组播结构体*/
    inet_pton(AF_INET, GROUP, &group.imr_multiaddr);                        /* 设置组播组地址*/
    inet_pton(AF_INET, "0.0.0.0", &group.imr_address);                      /*使用本地任意IP添加到组播组*/
    group.imr_ifindex = if_nametoindex("eth0");                             /* 设置网卡名 编号 ip ad */    
    #else
    struct ip_mreq group;
    group.imr_multiaddr.s_addr = inet_addr(argv[1]);  //GROUP
    group.imr_interface.s_addr = htonl(INADDR_ANY);
    #endif
    setsockopt(confd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &group, sizeof(group)); /* 将client加入组播组*/
	
    while (1) {
        len = recvfrom(confd, buf, sizeof(buf), 0, NULL, 0);
        /*len = recvfrom(confd, buf, sizeof(buf), 0, (struct sockaddr*)&localaddr, &addr_len);*/
        write(STDOUT_FILENO, buf, len);
    }
    close(confd);
    return 0;
}

