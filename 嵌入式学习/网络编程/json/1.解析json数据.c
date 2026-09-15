#include "myhead.h"
#include "cJSON.h"  //CJSON库的头文件

/*
	1.双引号嵌套双引号，里面的双引号必须要转义
*/
int main()
{
	//json数据
	// char *jsondata="{\"姓名\":\"马画藤\",\"年龄\":18}";
	// char *jsondata="{\"影片名字\":\"疯狂的石头\",\
	// 				\"票价\":29.9,\
	// 				\"上映时间\":\"2026-9-1\",\
	// 				\"影片的主要演员\":{\"男一号\":\"刘德华\",\"男二号\":\"梁朝伟\",\"女一号\":\"舒淇\"}}";

	char *jsondata="{\"影片名字\":\"疯狂的石头\",\
					\"票价\":29.9,\
					\"上映时间\":\"2026-9-1\",\
					\"影片的主要演员\":[{\"男一号\":\"刘德华\"},{\"男二号\":\"梁朝伟\"},{\"女一号\":\"舒淇\"}]}";

	//解析json
	//第一步：把字符串格式的json转换成cJSON存放到链表中
	/*
		cJSON是一个结构体类型，typedef取了别名得到的
		双向链表：cjson库解析json数据的原理
		          把对象中的键值对一个个拆分存放到双向链表的各个节点中
		
	*/
	//返回值：相当于获取到最外层的对象
	cJSON *obj=cJSON_Parse(jsondata);
	if(obj==NULL)
	{
		printf("解析json数据失败了\n");
		return -1;
	}
	
	//第二步：严格按照你肉眼看到的json数据的构成，一步步解析键值对
	cJSON *val1=cJSON_GetObjectItem(obj,"影片名字"); //我要获取obj这个对象中，姓名这个键对应的值
	printf("影片名字这个键对应的值是： %s\n",val1->valuestring);
	
	cJSON *val2=cJSON_GetObjectItem(obj,"票价"); //我要获取obj这个对象中，年龄这个键对应的值
	printf("票价这个键对应的值是： %f\n",val2->valuedouble);

	cJSON *val3=cJSON_GetObjectItem(obj,"上映时间"); //我要获取obj这个对象中，年龄这个键对应的值
	printf("上映时间这个键对应的值是： %s\n",val3->valuestring);

	// cJSON *chile_obj=cJSON_GetObjectItem(obj,"影片的主要演员"); //我要获取obj这个对象中，年龄这个键对应的值
	// cJSON *val4=cJSON_GetObjectItem(chile_obj,"男一号"); //我要获取obj这个对象中，年龄这个键对应的值
	// printf("男一号这个键对应的值是： %s\n",val4->valuestring);
	
	// cJSON *val5=cJSON_GetObjectItem(chile_obj,"男二号"); //我要获取obj这个对象中，年龄这个键对应的值
	// printf("男二号这个键对应的值是： %s\n",val5->valuestring);
	
	// cJSON *val6=cJSON_GetObjectItem(chile_obj,"女一号"); //我要获取obj这个对象中，年龄这个键对应的值
	// printf("女一号这个键对应的值是： %s\n",val6->valuestring);

	//数组实现
	cJSON *arr = cJSON_GetObjectItem(obj,"影片的主要演员");
    int size = cJSON_GetArraySize(arr);
    printf("数组元素个数：%d\n",size);

    //取下标0，数组里面是对象
    cJSON *item0 = cJSON_GetArrayItem(arr,0);
    cJSON *man1 = cJSON_GetObjectItem(item0,"男一号");
    printf("男一号：%s\n",man1->valuestring);

    cJSON *item1 = cJSON_GetArrayItem(arr,1);
    cJSON *man2 = cJSON_GetObjectItem(item1,"男二号");
    printf("男二号：%s\n",man2->valuestring);

    cJSON *item2 = cJSON_GetArrayItem(arr,2);
    cJSON *woman1 = cJSON_GetObjectItem(item2,"女一号");
    printf("女一号：%s\n",woman1->valuestring);

	cJSON_Delete(obj);
}