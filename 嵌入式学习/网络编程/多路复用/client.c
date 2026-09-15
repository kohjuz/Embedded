#include "myhead.h"

/*
	1.客户端：监测键盘和tcpsock
	2.select有如下几个特点
	     第一：阻塞的
		 第二：select如果监测多个文件描述符(fd1，fd2，fd3)，某一时刻如果fd1发生了状态改变，select会自动把没有发生状态改变的文件描述符
		 从集合中删除，只保留状态发生改变的文件描述符
*/

int main()
{
	int ret;
	int tcpsock;
	char sbuf[100];
	char rbuf[100];
	int otherret;

	//定义ipv4地址结构体变量，存放需要绑定的ip地址和端口号
	struct sockaddr_in bindaddr;
	bzero(&bindaddr,sizeof(bindaddr));
	bindaddr.sin_family=AF_INET;  //ipv4地址协议
	bindaddr.sin_addr.s_addr=htonl(INADDR_ANY); //自动匹配本地主机的ip地址
	bindaddr.sin_port=htons(10086); //程序员自己指定端口号，不要使用1024以内的端口号
	
	//定义ipv4地址结构体变量，存放服务器的ip和端口号
	struct sockaddr_in serveraddr;
	bzero(&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;  //ipv4地址协议
	serveraddr.sin_addr.s_addr=inet_addr("172.18.111.32"); //服务器的ip地址
	serveraddr.sin_port=htons(20000); //服务器的端口号
	
	//创建tcp套接字
	tcpsock=socket(AF_INET,SOCK_STREAM,0);
	if(tcpsock==-1)
	{
		perror("创建tcp套接字失败了\n");
		return -1;
	}
	
	//设置取消端口号绑定的限制
	int on=1; //类似于开关，非零表示开启对应的功能
	setsockopt(tcpsock,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	
	//绑定ip和端口号
	ret=bind(tcpsock,(struct sockaddr *)&bindaddr,sizeof(bindaddr));
	if(ret==-1)
	{
		perror("绑定ip和端口号失败了\n");
		return -1;
	}
	
	//连接服务器，由于我现在是自己跟自己通信，服务器，客户端都是我自己
	ret=connect(tcpsock,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
	if(ret==-1)
	{
		perror("连接服务器失败了\n");
		return -1;
	}
	
	//定义文件描述符集合变量，把所有你要监测的文件描述符存放到这个集合中
	fd_set myset;
    /*
    struct timeval {
               time_t      tv_sec;         
               suseconds_t tv_usec;        
           };
    */

	while(1)
	{

        //超时
        struct timeval timeout;
        timeout.tv_sec=5;

		//清空 
		FD_ZERO(&myset);
		//我要监测键盘
		FD_SET(0,&myset); //0就是键盘的文件描述符
		FD_SET(tcpsock,&myset); //套接字
		//调用select函数监测文件描述符
		ret=select(tcpsock+1,&myset,NULL,NULL,&timeout);
		if(ret>0) //文件描述符有数据可读
		{
			//由于select监测的文件描述符不止一个，因此你必须判断究竟是谁有数据可读
			if(FD_ISSET(0,&myset)==1)  //键盘在集合中--》说明是键盘有数据可读
			{
				bzero(sbuf,100);
				scanf("%s",sbuf);
				//发送数据给服务器
				send(tcpsock,sbuf,strlen(sbuf),0);			
			}
        
			if(FD_ISSET(tcpsock,&myset)==1)  //tcpsock在集合中--》说明是tcpsock有数据可读
			{
				bzero(rbuf,100);
				//接收数据
				otherret=recv(tcpsock,rbuf,100,0);
				if(otherret==0)
				{
					printf("服务器断开连接了\n");
					return -1;
				}
				printf("服务器发送过来的信息是: %s\n",rbuf);
			}
		}
        if(ret==0)
        {
            printf("连接超时了\n");
        }
		else
		{
			perror("监测失败了\n");
			return -1;
		}
	}
	
	//关闭套接字
	close(tcpsock);
	return 0;
}