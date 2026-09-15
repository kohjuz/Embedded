#include "myhead.h"
#include "cJSON.h"  //CJSON库的头文件

/*
	打包json数据
		
		创建数组
		cJSON *cJSON_CreateArray(void)
		创建对象
		cJSON *cJSON_CreateObject(void)
	
	添加bool类型到对象中
	cJSON_AddBoolToObject(对象,键的名字,true或者false)
	添加整数、小数到对象中
	cJSON_AddNumberToObject(对象,键的名字,整数或小数)  整数，小数都可以
	添加字符串到对象中
	cJSON_AddStringToObject(对象,键的名字,字符串)
	
	添加元素到数组
	cJSON_AddItemToArray(cJSON *array, cJSON *item);
	对象中添加键值对
	cJSON_AddItemToObject(cJSON *object, const char *string, cJSON *item);
	
	//带换行缩进输出
	char * cJSON_Print(const cJSON *item)
	//无空格换行输出，适合网络传输
	char * cJSON_PrintUnformatted(const cJSON *item)
	
{\"影片名字\":\"疯狂的石头\",\
					\"票价\":29.9,\
					\"上映时间\":\"2026-9-1\",\
					\"影片的主要演员\":[{\"男一号\":\"刘德华\"},{\"男二号\":\"梁朝伟\"},{\"女一号\":\"舒淇\"}]}
					"影片发行方:["华纳"，"华谊兄弟,"万达影业"]"
		
			*/
int main()
{
	//创建最外层的对象
	cJSON *obj=cJSON_CreateObject();
	cJSON *arr1=cJSON_CreateArray();
	cJSON *arr2=cJSON_CreateArray();
	//对象里面添加键值对
	cJSON_AddStringToObject(obj,"影片名字","疯狂的石头");
	cJSON_AddNumberToObject(obj,"票价",29.9);
	cJSON_AddStringToObject(obj,"上映时间","2026-9-1");
	
	cJSON *val=cJSON_CreateObject();
	cJSON_AddStringToObject(val,"男一号","刘德华");
	cJSON_AddItemToArray(arr1, val);

	cJSON *val2=cJSON_CreateObject();
	cJSON_AddStringToObject(val2,"男二号","梁朝伟");
	cJSON_AddItemToArray(arr1, val2);

	cJSON *val3=cJSON_CreateObject();
	cJSON_AddStringToObject(val3,"女一号","舒淇");
	cJSON_AddItemToArray(arr1, val3);

	cJSON_AddItemToArray(arr2, cJSON_CreateString("华纳"));
	cJSON_AddItemToArray(arr2, cJSON_CreateString("华谊兄弟"));
	cJSON_AddItemToArray(arr2, cJSON_CreateString("万达影业"));

	cJSON_AddItemToObject(obj,"影片的主要演员",arr1);
	cJSON_AddItemToObject(obj,"影片发行方",arr2);
	
	//输出
	//char *out=cJSON_Print(obj); //有换行，有缩进输出
	char *out=cJSON_PrintUnformatted(obj); //没有换行
	printf("%s\n",out);
	
	//释放
	cJSON_free(out);
	cJSON_Delete(obj);
	return 0;
}