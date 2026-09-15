#include "myhead.h"
#include <netinet/in.h>




int main(int argc,char **argv)
{
    
    struct sockaddr_in servaddr;
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(8080);
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);

    struct sockaddr_in otheraddr;
    int ip[20];
    int port;

    int udpsock=socket(AF_INET,SOCK_STREAM,0);
    if(udpsock==-1)
    {
        perror("socket fail");
        return -1;
    }
    //设置取消端口号绑定限制
    /*int setsockopt(int sockfd, int level, int optname,
                      const void optval[.optlen],
                      socklen_t optlen)
                      */
    int on =1;
    setsockopt(tcpsock,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));


    /*通用地址结构体(存放ip和端口)
    struct sockaddr_in ----->ipv4（32位）
    {
        sin_family:协议族(AF_INET)
        sin_port:端口号(网络字节序)
        sin_addr:ip地址(网络字节序)
    }
    struct sockaddr_in6 ----->ipv6（128位）
    struct sockaddr ----->通用地址结构体(兼容ipv4和ipv6)*/
    //int bind(int sockfd,const struct sockaddr *addr,socklen_t addrlen);
    
    //绑定ip和端口号
    int ret=bind(udpsock,(struct sockaddr *)&servaddr,sizeof(servaddr));
    if(ret==-1)
    {
        perror("bind fail");
        return -1;
    }
    char buf[1024]={0};
    while(1)
    {
        bzero(buf,sizeof(buf));
        bzero(&otheraddr,sizeof(otheraddr));
        printf("请输入ip和端口号\n");
        scanf("%s,%hu",ip,&port);
        printf("请输入发送的信息\n");
        scanf("%s",buf);
        otheraddr.sin_family=AF_INET;
        otheraddr.sin_addr=inet_addr(ip);
        otheraddr.sin_port=htons(port);
        sendto(udpsock,buf,stren(buf),0,(struct sockaddr_in *)&otheraddr,sizeof(otheraddr));

    }
    
    return 0;
}