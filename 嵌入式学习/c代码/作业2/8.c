#include<stdio.h>
#include<stdint.h>

void apt(uint32_t pkt);
void main()
{
    //0x12344520, 0xff004B1C, 0x00553C1E
    uint32_t arr[]={0x12344520, 0xff004B1C, 0x00553C1E};
    int n=sizeof(arr)/sizeof(arr[0]);
    for(int i=0;i<n;i++)
    {
        apt(arr[i]);

    }
}
void apt(uint32_t pkt)
{
    printf("当前数据为0x%08x\n",pkt);
    uint8_t wendu = pkt&0xFF;    //0-7最低8位，温度数据；
    uint8_t shidu = (pkt>>8)&0xFF; //湿度
    uint8_t door = (pkt>>16)&0x0F; //把前16位左移掉留下door4位&0x0f 0000 1111保留4个门状态
    uint8_t light = (pkt>>20)&0x0F; //4个灯状态

    printf("温度：%d,湿度%d\n,门0:%d,门1:%d,门2:%d,门3:%d\n,灯0:%d,灯1:%d,灯2:%d,灯3:%d\n"
        ,wendu,shidu,(door)&0x01,(door>>1)&0x01,(door>>2)&0x01,(door>>3)&0x01
        ,(light)&0x01,(light>>1)&0x01,(light>>2)&0x01,(light>>3)&0x01);
}