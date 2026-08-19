
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>



void main(int argc,char **argv)
{

   int fd =open("/dev/fb0",O_RDWR);
   if(fd<0)
   {
      perror("open");
      return;
   }
   struct fb_var_screeninfo var_info;
   ioctl(fd,FBIOGET_VSCREENINFO,&var_info);
   printf("var_info.xres =%d\n",var_info.xres);
   printf("var_info.yres =%d\n",var_info.yres);

   int *mp=mmap(NULL,var_info.xres*var_info.yres*4,PROT_WRITE,MAP_SHARED,fd,0);
   if(mp<0)
   {
      perror("mmap");
      return;
   }
   //#3300FF
   int color =0x3300FF;
   for(int i=0;i<var_info.xres*var_info.yres;i++)
   {
      mp[i]=color;
   }
   munmap(mp,var_info.xres*var_info.yres*4);



}