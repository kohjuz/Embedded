#include "myhead.h"
#include "cJSON.h"  
/*
	发送get请求获取天气预报
        http发送get请求的格式用C语言来写，应该如下: 
        GET /路径?query字段信息=值  HTTP/1.1\r\n      //多个字段用&分隔开
        Host:网址\r\n                                   (注意网址前面的https:\\不要写并且.com后面的路径也不要写)
        Authorization:APPCODE 验证码\r\n\r\n
        例如：
        char *httpreq="GET /kdi?no=777162614778849&type=STO HTTP/1.1\r\n"
        "Host: wuliu.market.alicloudapi.com\r\n"    //注意网址前面的https:\不要写
        "Authorization: APPCODE 295356acb9c34add9c0c3932fb22b693\r\n\r\n"
                
	     
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
	//https://ali-weather.showapi.com/weatherhistory
	struct hostent *urlmsg=gethostbyname("ali-weather.showapi.com");
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
	char *getreq="GET /weatherhistory?area=%E5%B9%BF%E5%B7%9E&month=202609 HTTP/1.1\r\n"
                "Host: ali-weather.showapi.com\r\n" 
                "Authorization: APPCODE cd6f9fc2b4a04336b87959c9edfdaccd\r\n\r\n";
	
	//发送这个请求
	send(tcpsock,getreq,strlen(getreq),0);
	
	//接收物流报服务器回复的应答信息
	recv(tcpsock,rbuf,10000,0);
	//printf("天气公司反馈给我信息是: %s\n",rbuf);
	//定位json起始 {
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

    cJSON *res_body = cJSON_GetObjectItem(root,"showapi_res_body");
    cJSON *list_arr = cJSON_GetObjectItem(res_body,"list");
    int arr_len = cJSON_GetArraySize(list_arr);

    printf("\n====广州天气解析结果====\n");
    for(int i=0;i<arr_len;i++)
    {
        cJSON *item = cJSON_GetArrayItem(list_arr,i);
        cJSON *area = cJSON_GetObjectItem(item,"area");
        cJSON *time = cJSON_GetObjectItem(item,"time");
        cJSON *weather = cJSON_GetObjectItem(item,"weather");
        cJSON *min_t = cJSON_GetObjectItem(item,"min_temperature");
        cJSON *max_t = cJSON_GetObjectItem(item,"max_temperature");
        cJSON *wind = cJSON_GetObjectItem(item,"wind_power");

        printf("地区:%s 日期:%s 天气:%s 最低:%s 最高:%s 风力:%s\n",
                area->valuestring,
                time->valuestring,
                weather->valuestring,
                min_t->valuestring,
                max_t->valuestring,
                wind->valuestring);
    }
	//关闭套接字
	close(tcpsock);
	return 0;
}