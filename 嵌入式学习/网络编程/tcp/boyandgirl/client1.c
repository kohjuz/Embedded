#include "myhead.h"
#include <netinet/in.h>



void *recv_thread(void *arg)
{
    int tcpsock=*(int *)arg;
    char buf[1024]={0};
    while(1)
    {
        bzero(buf,sizeof(buf));
        int ret=recv(tcpsock,buf,sizeof(buf),0);
        if(ret<=0)
        {
            printf("服务器关闭了\n");
            break;
        }
        if(strcmp(buf,"quit")==0)
        {
            printf("成功退出客户端\n");
            exit(0);
        }       
        printf("接收到服务器信息，接收到的信息是：%s\n",buf);
    }
    return NULL;
}


//客户端
int main(int argc,char **argv)
{
    //客户端IP和端口号
    struct sockaddr_in servaddr;
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(8080);
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    //servaddr.sin_addr.s_addr=inet_addr("192.168.44.144");
    //ubuntu小端序存放，计算机网络协议大端序
    //inet_addr()函数将点分十进制IP转换为网络字节序IP
    //htons()函数将主机字节序端口号转换为网络字节序端口号

    //创建tcp套接字
    int tcpsock=socket(AF_INET,SOCK_STREAM,0);
    if(tcpsock==-1)
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
    int ret=bind(tcpsock,(struct sockaddr *)&servaddr,sizeof(servaddr));
    if(ret==-1)
    {
        perror("bind fail");
        return -1;
    }

    //服务器IP和端口
    struct sockaddr_in cliaddr;
    bzero(&cliaddr,sizeof(cliaddr));
    cliaddr.sin_family=AF_INET;
    cliaddr.sin_port=htons(8888);
    //cliaddr.sin_addr.s_addr=inet_addr("47.99.216.34");
    cliaddr.sin_addr.s_addr=inet_addr("172.18.111.32");
    /*int connect(int sockfd, const struct sockaddr *addr,
                   socklen_t addrlen);*/

    //连接服务器
    int conret=connect(tcpsock,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
    if(conret==-1)
    {
        perror("connect fail");
        return -1;
    }
    printf("客户端 connect完成，socket = %d\n", tcpsock);

    /*键盘输入信息发给服务器
    第一组：write read
    第二组：send recv
    */

    //创建一个子线程接收,把socket传给子线程
    pthread_t tid;
    pthread_create(&tid,NULL,recv_thread,&tcpsock);


    char buf[1024]={0};
    int n=0;
    while(1)
    {
        // switch(n)
        // {
        //     //0,说明是 客户端向服务端发送信息
        //     case 0:    break;
        //     //1,说明是客户端给客户端发，但是还是要经过服务端，建立自己的通信协议
        //     case 1:    break;
        // }
        bzero(buf,sizeof(buf));
        printf("请输入要发送的信息：\n");
        scanf("%s",buf);
        send(tcpsock,buf,strlen(buf),0);
        if(strcmp(buf,"quit")==0)
        {
            printf("成功退出客户端\n");
            exit(0);
        }

    
    
    
    }
    close(tcpsock);
    
    return 0;
}