#include "myhead.h"

/*
	1.服务器：监测键盘和newsock
*/

int main()
{
	int tcpsock;
	int newsock; //存放新的套接字
	int ret;
	int otherret;
	char rbuf[100];
	char sbuf[100];

	//定义ipv4地址结构体变量，存放需要绑定的ip地址和端口号
	struct sockaddr_in bindaddr;
	bzero(&bindaddr,sizeof(bindaddr));
	bindaddr.sin_family=AF_INET;  //ipv4地址协议
	bindaddr.sin_addr.s_addr=htonl(INADDR_ANY); //自动匹配本地主机的ip地址
	bindaddr.sin_port=htons(20000); //程序员自己指定端口号，不要使用1024以内的端口号

	//定义ipv4地址结构体变量，存放目前连接成功的那个客户端信息
	struct sockaddr_in clientaddr;
	bzero(&clientaddr,sizeof(clientaddr));
	int len=sizeof(clientaddr);
	
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
	
	//监听
	ret=listen(tcpsock,5); //5表示最多允许5个客户端同时连接服务器
	if(ret==-1)
	{
		perror("监听失败了\n");
		return -1;
	}

	printf("服务器阻塞在accept的位置了\n");
	newsock=accept(tcpsock,(struct sockaddr *)&clientaddr,&len);
	if(newsock==-1)
	{
		perror("接受客户端的连接请求失败了\n");
		return -1;
	}
	
	//定义文件描述符集合变量，把所有你要监测的文件描述符存放到这个集合中
	fd_set myset;

	while(1)
	{
		//清空 
		FD_ZERO(&myset);
		//我要监测新的套接字
		FD_SET(newsock,&myset); 
		FD_SET(0,&myset);
		//调用select函数监测文件描述符
		ret=select(newsock+1,&myset,NULL,NULL,NULL);
		if(ret>0) //文件描述符有数据可读
		{
			if(FD_ISSET(newsock,&myset)==1)
			{
				bzero(rbuf,100);
				otherret=recv(newsock,rbuf,100,0);
				if(otherret==0)
				{
					printf("客户端断开了\n");
					return -1;
				}
				printf("客户端发送的信息是: %s\n",rbuf);
			}
			if(FD_ISSET(0,&myset)==1)
			{
				bzero(sbuf,100);
				scanf("%s",sbuf);
				//发送信息
				send(newsock,sbuf,strlen(sbuf),0);
			}
			
		}
		else
		{
			perror("监测失败了\n");
			return -1;
		}	
	}
	
	//关闭套接字
	close(tcpsock);
	close(newsock);
	return 0;
}