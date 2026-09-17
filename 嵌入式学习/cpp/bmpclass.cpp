#include <iostream>
#include <cstdint>
#include <cstring>
#include <fstream>

#pragma pack(push,1)

struct BmpFileHeader
{
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
};
// BMP淇℃伅澶� 40瀛楄妭
struct BmpInfoHeader
{
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};
#pragma pack(pop)

using namespace std;
//瀹氫箟rgb鍥惧儚绫�
class Image
{
public:
    Image():width(0),height(0),buf(NULL)
    {

    }
    Image(int w,int h)
    {
        width =w;
        height=h;
        buf =new uint8_t[width*height*3];
        memset(buf,0,width*height*3);

    }

    ~Image()
    {
        delete [] buf;
    }
    //閲婃斁鏃у唴瀛�
    void release()
    {
        if(buf)
        {
            delete [] buf;
            buf=NULL;
        }
        width=0;
        height=0;
    }
// 鎵撳紑鍔犺浇24浣岯MP鍥剧墖
    bool open(const char* path)
    {
        release(); // 鍏堥噴鏀句箣鍓嶇殑鍥剧墖

        ifstream file(path, ios::binary);
        if(!file.is_open())
        {
            cout << "鎵撳紑鏂囦欢澶辫触!" << endl;
            return false;
        }

        BmpFileHeader fh;
        BmpInfoHeader ih;
        file.read((char*)&fh, sizeof(fh));
        file.read((char*)&ih, sizeof(ih));

        if(fh.bfType != 0x4D42 || ih.biBitCount != 24)
        {
            cout << "鍙�鏀�鎸�24浣岯MP" << endl;
            file.close();
            return false;
        }

        width = ih.biWidth;
        height = ih.biHeight;

        // BMP姣忚�屽瓧鑺傛暟锛�4瀛楄妭瀵归綈
        int lineByte = (width * 3 + 3) / 4 * 4;
        buf = new uint8_t[width * height * 3];

        // BMP鏄�鍊掑簭瀛樺偍锛屽苟涓斿儚绱犻『搴� BGR
        file.seekg(fh.bfOffBits);
        for(int y = height -1; y >=0; y--)
        {
            for(int x = 0; x < width; x++)
            {
                uint8_t b,g,r;
                file.read((char*)&b,1);
                file.read((char*)&g,1);
                file.read((char*)&r,1);
                int idx = (y * width + x)*3;
                buf[idx + 0] = r;
                buf[idx + 1] = g;
                buf[idx + 2] = b;
            }
            // 璺宠繃姣忚�屾湯灏惧～鍏呯殑瀵归綈瀛楄妭
            file.seekg(lineByte - width*3, ios::cur);
        }
        file.close();
        cout << "鍥剧墖鍔犺浇鎴愬姛锛�" << width << " x " << height << endl;
        return true;
    }

    // 璁剧疆(x,y)鍍忕礌 RGB
    void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
    {
        // 杈圭晫鍒ゆ柇锛岄槻姝㈣秺鐣�
        if(x < 0 || x >= width || y <0 || y >= height)
            return;
        int idx = (y * width + x) * 3;
        buf[idx + 0] = r;
        buf[idx + 1] = g;
        buf[idx + 2] = b;
    }

    // 鑾峰彇(x,y)鍍忕礌RGB
    void getPixel(int x, int y, uint8_t& r, uint8_t& g, uint8_t& b)
    {
        if(x < 0 || x >= width || y <0 || y >= height)
        {
            r = g = b = 0;
            return;
        }
        int idx = (y * width + x) * 3;
        r = buf[idx + 0];
        g = buf[idx + 1];
        b = buf[idx + 2];
    }

    int width;
    int height;
    uint8_t* buf;



};

//灞忓箷绫�
class Screen
{
public:
    Screen(int w,int h)
    {
        scrw=w;
        scrh=h;
        scrbuf=new uint8_t[scrw*scrh*3];
        memset(scrbuf,0,scrw*scrh*3);
    }
    ~Screen()
    {
        delete [] scrbuf;
    }
    //灏嗗浘鐗囩粯鍒跺埌灞忓箷(offx,offy)浣嶇疆
    void drawImage(Image& img,int offsetx,int offsety)
    {
        uint8_t r,g ,b;
        for(int y=0;y<img.height;y++)
        {
            for(int x=0;x<img.width;x++)
            {
                int px=offsetx+x;
                int py=offsety+y;
                if(px<0||px>=scrw||py<0||py>=scrh)
                    continue;
                img.getPixel(x,y,r,g,b);
                int scrIdx=(py*scrw+px)*3;
                scrbuf[scrIdx+0]=r;
                scrbuf[scrIdx+1]=g;
                scrbuf[scrIdx+2]=b;
            }
        }
        cout <<"鎵撳紑鍥剧墖鎴愬姛"<<endl;
    }

private:
    int scrw;
    int scrh;
    uint8_t* scrbuf;

};

int main()
{
    Image img;
    img.open("test.bmp");
    Screen scr(img.width,img.height);
    scr.drawImage(img,0,0);
    return 0;
}