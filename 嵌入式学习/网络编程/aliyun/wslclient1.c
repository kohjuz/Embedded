#include "myhead.h"
#include <netinet/in.h>



void *recv_thread(void *arg)
{
    int tcpsock=*(int *)arg;
    free(arg);
    char buf[1024]={0};
    msg_t msg;
    while(1)
    {
        bzero(&msg,sizeof(msg));
        int ret=recv(tcpsock,&msg,sizeof(msg),0);
        if(ret<=0)
        {
            printf("服务器关闭了\n");
            break;
        }
        if(strcmp(msg.buf,"quit")==0)
        {
            printf("成功退出客户端\n");
            exit(0);
        }       
        printf("接收到服务器信息，接收到的信息是：%s\n",msg.buf);
    }
    return NULL;
}


//客户端
int main(int argc,char **argv)
{


    //创建tcp套接字
    int tcpsock=socket(AF_INET,SOCK_STREAM,0);
    if(tcpsock==-1)
    {
        perror("socket fail");
        return -1;
    }

    //服务器IP和端口
    struct sockaddr_in cliaddr;
    bzero(&cliaddr,sizeof(cliaddr));
    cliaddr.sin_family=AF_INET;
    cliaddr.sin_port=htons(8888);
    //cliaddr.sin_addr.s_addr=inet_addr("47.99.216.34");//阿里云服务器
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
    int *pdf=malloc(sizeof(int));
    *pdf=tcpsock;

    pthread_create(&tid,NULL,recv_thread,pdf);


    char buf[1024]={0};
    int n;

    while(1)
    {
        printf("请选择模式：0.客户端向服务端发送信息，1.客户端给客户端发信息\n");
        scanf("%d",&n);
        switch(n)
        {
            //0,说明是 客户端向服务端发送信息
            case 0:
            {
                bzero(buf,sizeof(buf));
                printf("请输入要发送的信息：\n");
                scanf("%s",buf);

                if(strcmp(buf,"quit")==0)
                {
                    printf("成功退出客户端\n");
                    exit(0);
                }
                msg_t msg;
                msg.mode=n;
                strcpy(msg.buf,buf);
                send(tcpsock,&msg,sizeof(msg),0);
            }
            break;
            //1,说明是客户端给客户端发，但是还是要经过服务端，建立自己的通信协议
            case 1:
            {
                bzero(buf,sizeof(buf));
                printf("请以(ip@端口@信息)格式输入\n");
                scanf("%s",buf);
                if(strcmp(buf,"quit")==0)
                {
                    printf("成功退出客户端\n");
                    exit(0);
                }
                msg_t msg;
                msg.mode=n;
                strcpy(msg.buf,buf);
                send(tcpsock,&msg,sizeof(msg),0);

            }
            break;
        }



    
    
    
    }
    close(tcpsock);
    
    return 0;
}