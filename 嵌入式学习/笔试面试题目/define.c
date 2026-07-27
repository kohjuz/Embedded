/*
# 二、编程
## 1、C/C++：完成下面的宏定义，假设获取当前时间字符串的函数为 `get_local_time()`
### 其一
```c
#define EPRINTF(func, fmt, args...)  (\
)
```
调用示例：
`EPRINTF(test(10), "->%s: %d\n", "hello", 2345);`
输出要求：
`当前时间 [调用的行号 @ 所在原文件名]: test(10), ->hello: 2345`

### 其二
```c
#define WHEN(func, act) { \
}
```
调用示例：
`WHEN(test_failed(), return -1);`
逻辑与输出要求：
调用 `test_failed()`，如果该函数返回不为 0，则打印日志信息并执行动作 `act`；
日志输出格式：
`当前时间 [调用的行号 @ 所在原文件名]: test_failed(), return -1`*/
// #include <stdio.h>
// #include <time.h>
// #include <stdlib.h>
// #define EPRINIF(func, fmt, args...) { \
//     time_t t=time(NULL); \
//     printf("%s [%d@%s] : %s",strtod((t),'\n'),__LINE__,__FILE__,#func); \
//     printf(fmt,##args);\
// }
// int main(int argc,char **argv)
// {

//     EPRINIF(test(10),"->%s: %d\n","hello",2345);
//     return 0;
// }
#include <stdio.h>
#define USART_RATE 115200

#define _STR(x) #x
#define STR(x) _STR(x)
void main(void)
{
    printf("usart rate = %s\n",STR(USART_RATE));
    
}
