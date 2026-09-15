#include "myhead.h"


ClientNode *head=NULL;//没有数据让他指向空
pthread_mutex_t mutex;//互斥锁，保护链表，防止多线程访问混乱

//删除结点函数
void delete_node(ClientNode **head,ClientNode *del)
{
    ClientNode *pre=NULL;
    for(ClientNode *p=*head;p!=NULL;p=p->next)
    {
        if(p == del)
        {
            if(pre==NULL)
            {
                *head=p->next;
            }
            else
            {
                pre->next=p->next;
            }
            
            return;

        }
        pre=p;
    }
}


//发送线程，发给客户端
//arg传链表结点node的地址

void *send_msg(void *arg)
{
    //int  psock=*((int *)arg);
    ClientNode *node=(ClientNode *)arg;
    int psock=node->sockfd;
    char buf[1024]={0};
    char ipbuf[1024]={0};
    char portbuf[1024]={0};
    while(1)
    {
        bzero(buf,sizeof(buf));
        msg_t msg;
        bzero(&msg,sizeof(msg));
        printf("请输入发送模式：（0单独发给客户端）（1群发）\n");
        scanf("%d",&msg.mode);
        if(msg.mode==0)
        {
            
            printf("请输入要发送的客户端ip和端口号：\n");
            scanf("%s %d",ipbuf,portbuf);
            pthread_mutex_lock(&mutex);
            int statefd=-1;
            for(ClientNode *p=head;p!=NULL;p=p->next)
            {
                if(strcmp(p->ip,ipbuf)==0&&p->port==atoi(portbuf))
                {
                    statefd=p->sockfd;
                    break;
                }
            }
            pthread_mutex_unlock(&mutex);
            printf("请输入要发送的消息：\n");
            scanf("%s",msg.buf);
            send(statefd,&msg,sizeof(msg),0);
        }
        else if(msg.mode==1)
        {
            printf("请输入要发送的消息：\n");
            scanf("%s",msg.buf);
            pthread_mutex_lock(&mutex);
            for(ClientNode *p=head;p!=NULL;p=p->next)
            {
                send(p->sockfd,&msg,sizeof(msg),0);
            }
            pthread_mutex_unlock(&mutex);

           
        }
        else
        {
            printf("输入错误，请重新输入\n");
        }

        
        
        printf("发送给客户端[%s,%d]\n", node->ip, node->port);
        scanf("%s",buf);
        send(psock,buf,strlen(buf),0);
        if(strcmp(buf,"quit")==0)
        {
            printf("成功退出服务端\n");            
            close(psock);
            //退出的话要删除结点，释放内存，退出线程
            pthread_mutex_lock(&mutex);
            delete_node(&head,node);
            pthread_mutex_unlock(&mutex);

            free(node);
            pthread_exit(NULL);
        }
        
    }
}

//接收子线程，接收客户端发送的信息
void *recv_msg(void *arg)
{
    char buf[1024]={0};
    ClientNode *node=(ClientNode *)arg;
    int psock=node->sockfd;
    msg_t msg;

    while(1)
    {
        bzero(&msg,sizeof(msg));
        bzero(buf,sizeof(buf));
        
        int n = recv(psock, &msg, sizeof(msg), 0);
        printf("DEBUG: recv return n=%d , expect=%ld\n", n, sizeof(msg));
        printf("DEBUG buf=[%s]\n", msg.buf);
        if(n <= 0)
        {
            printf("客户端断开连接\n");
            close(psock);
            pthread_mutex_lock(&mutex);
            delete_node(&head,node);
            pthread_mutex_unlock(&mutex);

            free(node);
            pthread_exit(NULL);            
        }
        
        if(strcmp(msg.buf,"quit")==0)
        {
            printf("成功退出服务端\n");
            pthread_mutex_lock(&mutex);
            delete_node(&head,node);
            pthread_mutex_unlock(&mutex);

            free(node);
            pthread_exit(NULL);
        }
        if(msg.mode==0)
        {
            printf("mode0,收到客户端[%s:%d]消息：%s\n",node->ip,node->port,msg.buf);
        }
        else if(msg.mode==1)
        {
            printf("mode1,转发模式[%s:%d]消息：%s\n",node->ip,node->port,msg.buf);
            //解析信息
            //(ip@端口@信息)
            char temp[1024]={0};
            strcpy(temp,msg.buf);
            char *save=NULL;
            char *ip=strtok_r(temp,"@",&save);
            char *port=strtok_r(NULL,"@",&save);
            char *msg_data=strtok_r(NULL,"@",&save);
            if(ip==NULL||port==NULL||msg_data==NULL)
            {
                printf("解析失败,请按照正确格式输入\n");
                continue;
            }
            printf("ip:%s,port:%d,msg:%s\n",ip,atoi(port),msg_data);
            //找到ip和port对应的客户端
            //访问共享链表head，必须加锁
            pthread_mutex_lock(&mutex);
            int statefd=-1;
            for(ClientNode *p=head;p!=NULL;p=p->next)
            {
                if(strcmp(p->ip,ip)==0&&p->port==atoi(port))
                {
                    statefd=p->sockfd;
                    break;
                }
            }
            pthread_mutex_unlock(&mutex);
            if(statefd>0)
            {
                msg_t msg;
                msg.mode=0;
                strcpy(msg.buf,msg_data);
                send(statefd,&msg,sizeof(msg),0);
                printf("转发成功\n");
            }
            else
            {
                printf("没有找到客户端\n");
            
            }
        }
    
    }
}

int newtcpsock;
//服务器端
int main(int argc,char **argv)
{
    pthread_mutex_init(&mutex,NULL);//初始化互斥锁
    //服务器IP和端口号
    struct sockaddr_in servaddr;
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(8888);
    //htonl(INADDR_ANY)自动匹配本地主机的ip地址
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    // servaddr.sin_addr.s_addr=inet_addr("192.168.44.144");


    //创建tcp套接字
    int tcpsock=socket(AF_INET,SOCK_STREAM,0);
    if(tcpsock==-1)
    {
        perror("socket fail");
        return -1;
    }

    int on =1;
    setsockopt(tcpsock,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
    
    int ret=bind(tcpsock,(struct sockaddr *)&servaddr,sizeof(servaddr));
    if(ret==-1)
    {
        perror("bind fail");
        return -1;
    }
    printf("服务端 bind完成，socket = %d\n", tcpsock);
    
    //待机监听
    //int listen(int sockfd, int backlog);
    //backlog:最大连接数(允许多少个客户端同时连接)
    int retlis=listen(tcpsock,5);
    if(retlis==-1)
    {
        perror("listen fail");
        return -1;
    }
    printf("服务端 listen完成，socket = %d\n", tcpsock);

    //愿意接听 accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
    //sockfd:监听套接字
    //addr:客户端的ip和端口号
    //addrlen:客户端的ip和端口号的长度
    //返回值:成功的话返回新的套接字，用于和客户端通信
    //int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    //accept();若没有连接到客户端，则阻塞等待


    //接受客户端的连接请求
    while(1)
    {
    struct sockaddr_in cliaddr;
    bzero(&cliaddr,sizeof(cliaddr));
    socklen_t clilen=sizeof(cliaddr);
    newtcpsock=accept(tcpsock,(struct sockaddr *)&cliaddr,&clilen);
    if(newtcpsock==-1)
    {
        perror("accept fail");
        return -1;
    }
    printf("服务端 accept完成，newsocket = %d\n", newtcpsock);
    printf("ip:%s 端口号 %d\n",inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port));

    //新建链表结点
    ClientNode *node=malloc(sizeof(ClientNode));
    node->sockfd=newtcpsock;//保存新的fd
    strcpy(node->ip,inet_ntoa(cliaddr.sin_addr));
    node->port=ntohs(cliaddr.sin_port);

    //头插链表
    pthread_mutex_lock(&mutex);//上锁
    node->next=head; 
    head=node;
    pthread_mutex_unlock(&mutex);


    //写一个子线程发送消息给客户端
    // pthread_t send_tid;
    // pthread_create(&send_tid,NULL,send_msg,node);
    // pthread_detach(send_tid);

    //写一个接收信息子线程
    pthread_t recv_tid;
    pthread_create(&recv_tid,NULL,recv_msg,node);
    pthread_detach(recv_tid);


    

    
    }
    //销毁锁
    pthread_mutex_destroy(&mutex);
    close(tcpsock);


    return 0;
}