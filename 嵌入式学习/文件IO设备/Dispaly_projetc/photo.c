#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/input.h>      
#include <stdbool.h>
#include <dirent.h>
#include <string.h>
#include <strings.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>


#include "jpeg_display.h"

struct bitmap_header
{
	int16_t type;
	int32_t size; // 图像文件大小
	int16_t reserved1;
	int16_t reserved2;
	int32_t offbits; // bmp图像数据偏移量
}__attribute__((packed));

struct bitmap_info
{
	int32_t size;   // 本结构大小	
	int32_t width;  // 图像宽
	int32_t height; // 图像高
	int16_t planes;

	int16_t bit_count; // 色深
	int32_t compression;
	int32_t size_img; // BMP文件中的RGB数据大小，必须是4的整数倍
	int32_t X_pel;
	int32_t Y_pel;
	int32_t clrused;
	int32_t clrImportant;
}__attribute__((packed));

// 以下结构体不一定存在于BMP文件中，除非：
// bitmap_info.compression为真
struct rgb_quad
{
	int8_t blue;
	int8_t green;
	int8_t red;
	int8_t reserved;
}__attribute__((packed));




int tp;

// 保存查找到的照片文件名
static char photo_list[256][256];//
static int photo_count = 0;

// 判断文件名是否为图片(.jpg/.jpeg/.bmp)，是返回1，否则返回0
static int is_photo_file(const char *name)
{
    // 排除以 . 开头的隐藏文件(如 . ..)，再判断后缀
    if(name == NULL || name[0] == '.')
        return 0;

    const char *p = strrchr(name, '.');
    if(p == NULL)
        return 0;

    if(strcasecmp(p, ".jpg") == 0 || strcasecmp(p, ".jpeg") == 0
       || strcasecmp(p, ".bmp") == 0)
        return 1;

    return 0;
}
// 判断文件名是 JPG 还是 BMP
// 返回: 1=JPG, 2=BMP, 0=都不是
int is_jpg_or_bmp(const char *name)
{
    if(name == NULL)
        return 0;

    const char *p = strrchr(name, '.');
    if(p == NULL)
        return 0;

    if(strcasecmp(p, ".jpg") == 0 || strcasecmp(p, ".jpeg") == 0)
        return 1;  // JPG
    if(strcasecmp(p, ".bmp") == 0)
        return 2;  // BMP

    return 0;
}

// 返回收集到的图片列表(指向全局 photo_list 的指针)
// 通过 out_count 输出图片数量
char (*find_photo(int *out_count))[256]
{
    /* 直接打开 ./lib，不再 chdir，避免污染当前工作目录 */
    DIR *dir = opendir("./lib_photo");
    if(dir == NULL)
    {
        perror("opendir ./lib_phoro");
        if(out_count) *out_count = 0;
        return NULL;
    }

    photo_count = 0;
    struct dirent *ent;
    while((ent = readdir(dir)) != NULL)
    {
        if(!is_photo_file(ent->d_name))
        {
            // 非图片文件直接跳过，不打印干扰信息
            continue;
        }

        if(photo_count < 256)
        {
            // 跳过与上一张同名的文件(避免重复)
            if(photo_count > 0 &&
               strcmp(ent->d_name, photo_list[photo_count-1]) == 0)
            {
                continue;
            }
            /* 拼接 "./lib/" 前缀,后续 photo_output 可直接以该路径打开 */
            snprintf(photo_list[photo_count], 256, "./lib_photo/%s", ent->d_name);
            printf("found: %s (%s)\n",
                   photo_list[photo_count],
                   is_jpg_or_bmp(photo_list[photo_count]) == 1 ? "JPG" :
                   is_jpg_or_bmp(photo_list[photo_count]) == 2 ? "BMP" : "?");
            photo_count++;
        }
        else
        {
            printf("photo list full, ignore: %s\n", ent->d_name);
        }
    }
    closedir(dir);

    printf("total photos: %d\n", photo_count);
    if(photo_count == 0)
    {
        printf("no photo in current directory\n");
        if(out_count) *out_count = 0;
        return NULL;
    }
    if(out_count) *out_count = photo_count;
    return photo_list;
}
//清屏函数清除BMP
// void clear_photo()
// {

// }

void bmp_display(const char *name)
{
    int lcd = open("/dev/fb0", O_RDWR);
    if(lcd == -1)
    {
        perror("open lcd failed");
        return;
    }

    struct fb_var_screeninfo vinfo;
    ioctl(lcd, FBIOGET_VSCREENINFO, &vinfo);

    int lcd_w = vinfo.xres;
    int lcd_h = vinfo.yres;
    int lcd_bpp = vinfo.bits_per_pixel;
    int lcd_line_size = lcd_w * lcd_bpp / 8;
    int lcd_size = lcd_w * lcd_h * lcd_bpp / 8;

    char *p = mmap(NULL, lcd_size, PROT_READ|PROT_WRITE, MAP_SHARED, lcd, 0);
    if(p == MAP_FAILED)
    {
        perror("mmap lcd failed");
        return;
    }

    // 读取bmp文件格式头
    int fd = open(name, O_RDONLY);
    if(fd == -1)
    {
        perror("open bmp failed");
        return;
    }

    // 读取bmp文件RGB数据
    struct bitmap_header header;
    struct bitmap_info   info;
    bzero(&header, sizeof(header));
    bzero(&info, sizeof(info));

    read(fd, &header, sizeof(header));
    read(fd, &info, sizeof(info));

    if(info.compression != 0)
    {
        struct rgb_quad quad;
        bzero(&quad, sizeof(quad));
        read(fd, &quad, sizeof(quad));
    }

    int bmp_w = info.width;
    int bmp_h = info.height;
    int bmp_bpp = info.bit_count;

    printf("图片分辨率:%d×%d\n", info.width, info.height);

    // 计算一行中的无效字节数
    int pad = (4 - (bmp_w*bmp_bpp/8 % 4)) % 4;
    int bmp_line_size = bmp_w * bmp_bpp / 8+pad;
    int bmp_size = bmp_line_size * bmp_h;

    char *rgb = malloc(bmp_size);
    bzero(rgb, bmp_size);

    // 读取bmp文件RGB数据
    while(bmp_size > 0)
    {
        int n = read(fd, rgb, bmp_size); // n <= bmp_size
        if(n == -1)
        {
            perror("read bmp failed");
            return;
        }
        bmp_size -= n;
    }

    // 将RGB数据写入LCD
    int pixel = bmp_bpp/8;

    // 居中偏移：图片比屏幕小则为正，比屏幕大则为负(配合越界判断裁剪)
    int off_x = (lcd_w - bmp_w) / 2;
    int off_y = (lcd_h - bmp_h) / 2;

// 将整张BMP画上LCD
// j：BMP图片行号(从上往下，0~bmp_h-1)
for(int j=0; j<bmp_h; j++)
{
    // 计算图片第j行对应的LCD行号
    int y_lcd = off_y + j;
    if(y_lcd < 0 || y_lcd >= lcd_h)
        continue;   // 越界行跳过(大图时裁掉屏幕外的部分)

    int lcd_offset = lcd_line_size * y_lcd;

    /*
     BMP倒序：
     lcd第j行(屏幕上面) → 对应bmp的 (bmp_h?1?j) 行
     bmp_offset：取带pad的bmp行偏移
    */
    int bmp_row = bmp_h - 1 - j;
    int bmp_offset = bmp_line_size * bmp_row;

    // 将图片的一行画上LCD
    for(int i=0; i<bmp_w; i++)
    {
        // 计算图片第i列对应的LCD列号
        int x_lcd = off_x + i;
        if(x_lcd < 0 || x_lcd >= lcd_w)
            continue;   // 越界列跳过

        memcpy(p + 4*x_lcd + lcd_offset,
               rgb + i*pixel + bmp_offset,
               pixel);
    }
    // clear_photo(&fd);
}


    // 释放资源
    munmap(p, lcd_size);
    close(lcd);
    close(fd);
    free(rgb);
}

void photo_output(const char *name)
{
    //判断照片是JPG还是BMP
    int type = is_jpg_or_bmp(name);
    if(type == 1)
    {
        //是jpg就用#include "jpeg_display.h"
        //jpeg转化rgb
        struct imageInfo img;
        if(jpg2rgb(name,&img)!=0)
            return;

    FBInfo fb;
    if (fb_open(&fb) != 0) {
        free(img.rgb);
        return;
    }
    fb_clear(&fb);

    /* 计算缩放比例: 默认原尺寸; fit = 等比缩放铺满屏幕内 */
    int scale = 1000;   /* 1000 = 100% */
    
        int s_x = fb.width  * 1000 / img.width;
        int s_y = fb.height * 1000 / img.height;
        scale = (s_x < s_y) ? s_x : s_y;
        if (scale < 1) scale = 1;
    

    /* 函数2: RGB24 → 显示到屏幕 */
    fb_show_image(&fb, &img, scale);

    /* 清理 */
    fb_close(&fb);
    free(img.rgb);
    return;
    }
    else if(type == 2)
    {
        //是bmp
        bmp_display(name);
    }
    else
    {
        printf("unknown type: %s\n", name);
    }
}

int click()
{
    int x1,y1,x2,y2;
    bool xldone =false;
    bool yldone =false;


    struct input_event buf={0};
    while(1)
    {
        read(tp,&buf,sizeof(buf));
        if(buf.type==EV_ABS&&buf.code==ABS_X&&!xldone)
        {
            x1=x2=buf.value;
            xldone=true;
        }
        if(buf.type==EV_ABS&&buf.code==ABS_Y&&!yldone)
        {
            y1=y2=buf.value;
            yldone=true;
        }
        if(buf.type==EV_ABS&&buf.code==ABS_X&&xldone)
        {
            x2=buf.value;
        }
        if(buf.type==EV_ABS&&buf.code==ABS_Y&&yldone)
        {
            y2=buf.value;
        }
        if(buf.type==EV_KEY&&buf.code==BTN_TOUCH&&buf.value==0)
        {
            if(abs(x1-x2)<20&&abs(y1-y2)<20)
            {
                int clink_x=x2;
                
                int lcd_middle=1024/2;
                if(clink_x<lcd_middle)
                {
                    return 1;
                }
                else
                {
                    return 2;
                }
            }
            else
            {
                xldone=false;
                yldone=false;
            }
        }
    }

    
}





int main(int argc,char **argv)
{
    tp=open("/dev/input/event6",O_RDWR);
    if(tp<0)
    {
        perror("open");
        return -1;
    }
    int count = 0;
    char (*list)[256] = find_photo(&count);
    if(list == NULL)
    {
        printf("no photos, exit\n");
        return -1;
    }

    int idx = 0;  // 当前显示的图片下标
    photo_output(list[idx]);
    while(1)
    {
        int n = click();//点击1左2右
        if(n==1)
        {
            idx = (idx - 1 + count) % count;
        }
        else if(n==2)
        {
            idx = (idx + 1) % count;
        }
        else 
        {
            continue;
        }

        // 显示当前图片
        photo_output(list[idx]);
        // 点击后切换到下一张(循环)
        // idx = (idx + 1) % count;
    }

    return 0;
}
