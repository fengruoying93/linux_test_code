#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>
 
#define SERVER_PORT 8000
#define CLIENT_PORT 9000
#define MAXLINE 1500
#define GROUP "239.0.0.2"
//239.0.0.0��239.255.255.255
 
int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in serveraddr, clientaddr;
    char buf[MAXLINE] = "multicast\n";
    struct ip_mreqn group;
 
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);                /*��������UDPͨ�ŵ��׽���*/
    
    //����˲���Ҫ����ಥ��,����ֱ����ಥ�鷢������
    #if 0
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;                        /* IPv4 */
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);         /*��������IP INADDR_ANY = 0 */
    serveraddr.sin_port = htons(SERVER_PORT);
 
    bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
 
    inet_pton(AF_INET, GROUP, &group.imr_multiaddr);        /*�����鲥��ĵ�ַ*/
    inet_pton(AF_INET, "0.0.0.0", &group.imr_address);      /* ��������IP �Զ�������ЧIP*/
    group.imr_ifindex = if_nametoindex("eth0");             /* ������������ת��Ϊ��Ӧ��ţ�eth0 --> ���         ����  ����:ip ad */
 
    setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, &group, sizeof(group));  /*��ȡ�鲥Ȩ��*/
    #endif
 
    bzero(&clientaddr, sizeof(clientaddr));                 /* ����client ��ַ IP+�˿ں�*/
    clientaddr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &clientaddr.sin_addr.s_addr); /* IPv4  239.0.0.2+9000 */  //GROUP
    clientaddr.sin_port = htons(atoi(argv[2]));  //CLIENT_PORT
 
    int i = 0;
    while (1) {
        sprintf(buf, "multicast %d\n", i++);
        //fgets(buf, sizeof(buf), stdin);
		printf("send multicast %d..\n", i);
        sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&clientaddr, sizeof(clientaddr));
        sleep(1);
    }
    close(sockfd);
    return 0;
}

