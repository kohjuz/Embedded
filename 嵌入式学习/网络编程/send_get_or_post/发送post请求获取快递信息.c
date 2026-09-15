#include "myhead.h"
#include "cJSON.h"

/*
	发送get请求获取天气预报
	   2.需要注意的问题
	       问题1：完整的网址  https://kzexpress.market.alicloudapi.com/api-mall/api/express/query
		          完整的网址分成三个部分：
				          https://  协议头
						  kzqtqyb.market.alicloudapi.com  网址
						  /api/weather/one/forty  路径名
		          写代码：去掉https://，去掉.com后面的内容
		   
	     
*/
int main()
{
	int ret;
	int tcpsock;
	char rbuf[10000]={0};
	
	//定义ipv4地址结构体变量，存放需要绑定的ip地址和端口号
	struct sockaddr_in bindaddr;
	bzero(&bindaddr,sizeof(bindaddr));
	bindaddr.sin_family=AF_INET;  //ipv4地址协议
	bindaddr.sin_addr.s_addr=htonl(INADDR_ANY); //自动匹配本地主机的ip地址
	bindaddr.sin_port=htons(10086); //程序员自己指定端口号，不要使用1024以内的端口号
	
	//获取物流服务器ip地址
	//写代码：去掉https://，去掉.com后面的内容
	//https://kzexpress.market.alicloudapi.com/api-mall/api/express/query
	struct hostent *urlmsg=gethostbyname("kzexpress.market.alicloudapi.com");
	//获取ip地址
	//第一步：二级指针解引用，变成char *
	char *ippoint=*(urlmsg->h_addr_list);
	//第二步：把char*强制转换成struct in_addr*
	struct in_addr *otherip=(struct in_addr *)ippoint;
	//第三步：把转换后的当成参数传递
	char *trueip=inet_ntoa(*otherip);
	
	
	//定义ipv4地址结构体变量，存放服务器的ip和端口号
	struct sockaddr_in serveraddr;
	bzero(&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;  //ipv4地址协议
	serveraddr.sin_addr.s_addr=inet_addr(trueip); //天气预报服务器的ip地址
	serveraddr.sin_port=htons(80); //服务器的端口号,http端口号固定是80
	
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
	
	//连接物理服务器
	ret=connect(tcpsock,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
	if(ret==-1)
	{
		perror("连接服务器失败了\n");
		return -1;
	}
	
	//发送post请求获取物流
	/*
		重点关注：post请求书写格式
		Content-Type:application/x-www-form-urlencoded
		    指定post请求内容格式：form表示表单格式
			表单格式的含义：
			      adcode=440117000000
			      
	*/
	char *postreq="POST /api-mall/api/express/query HTTP/1.1\r\n"
				  "Host:kzexpress.market.alicloudapi.com\r\n"
				  "Content-Type:application/x-www-form-urlencoded\r\n"
				  //19表示body字段实际长度
				  "Content-Length:25\r\n"  
				  "Authorization: APPCODE cd6f9fc2b4a04336b87959c9edfdaccd\r\n\r\n"
				  "expressNo=465651602973731";  //body字段中的内容
	
	//发送这个请求
	send(tcpsock,postreq,strlen(postreq),0);
	
	//接收物流报服务器回复的应答信息
	recv(tcpsock,rbuf,10000,0);
	//printf("快递公司反馈给我信息是: %s\n",rbuf);
	
	//获得数据
	    char *json_begin = strstr(rbuf,"{");
    if(json_begin == NULL)
    {
        printf("未找到{\n");
        close(tcpsock);
        return -1;
    }
	char *json_end = strrchr(rbuf,'}');
    if(json_end == NULL)
    {
        printf("未找到}\n");
        close(tcpsock);
        return -1;
    }
	int json_len = json_end - json_begin + 1;
	char json_start[10000]={0};
	memcpy(json_start,json_begin,json_len);
	

    cJSON *root = cJSON_Parse(json_start);
    if(root == NULL)
    {
        printf("cJSON解析失败\n");
        close(tcpsock);
        return -1;
    }


	//========解析返回的圆通JSON========
    cJSON *msg = cJSON_GetObjectItem(root,"msg");
    cJSON *code = cJSON_GetObjectItem(root,"code");
    cJSON *data = cJSON_GetObjectItem(root,"data");

    cJSON *cpCode = cJSON_GetObjectItem(data,"cpCode");
    cJSON *mailNo = cJSON_GetObjectItem(data,"mailNo");
    cJSON *company = cJSON_GetObjectItem(data,"logisticsCompanyName");
    cJSON *lastMsg = cJSON_GetObjectItem(data,"theLastMessage");
    cJSON *lastTime = cJSON_GetObjectItem(data,"theLastTime");

    printf("\n====快递解析结果====\n");
    printf("返回信息：%s\n",msg->valuestring);
    printf("状态码：%d\n",code->valueint);
    printf("快递公司：%s\n",company->valuestring);
    printf("快递单号：%s\n",mailNo->valuestring);
    printf("最新时间：%s\n",lastTime->valuestring);
    printf("最新物流：%s\n",lastMsg->valuestring);

    //解析数组 logisticsTraceDetailList
    cJSON *traceArr = cJSON_GetObjectItem(data,"logisticsTraceDetailList");
    int arrSize = cJSON_GetArraySize(traceArr);
    printf("\n====物流明细====\n");
    for(int i = 0; i < arrSize; i++)
    {
        cJSON *item = cJSON_GetArrayItem(traceArr,i);
        cJSON *timeDesc = cJSON_GetObjectItem(item,"timeDesc");
        cJSON *desc = cJSON_GetObjectItem(item,"desc");
        printf("[%s] %s\n", timeDesc->valuestring, desc->valuestring);
    }

	//关闭套接字
	close(tcpsock);
	return 0;
}