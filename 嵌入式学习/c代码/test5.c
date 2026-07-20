#include<stdio.h>
struct smart_home sh = {0};
sh.fan   = 0;  
sh.light = 6;  
sh.temp  = 26; 
void main()
{


printf("%ld\n", *(long *)(&sh));
}