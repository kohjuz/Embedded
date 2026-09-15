#include <iostream>
#include <cstring>     // strcpy, strlen
#include <fcntl.h>     // open
#include <unistd.h>    // close

using namespace std;

class File
{
    public:
    File(const char *path,int _size,string _name)
    {
        filepath=new char[strlen(path)+1];
        strcpy(filepath,path);
        
    
        size=_size;
        name=_name;
        fd=open(filepath,O_RDONLY);
        if(fd<0)
        {
            cerr<<"打开文件失败:"<<filepath<<endl;

        }
        else
        {
            cerr<<"打开文件成功："<<filepath<<endl;
        }

    }
    ~File()
    {
        if(fd>=0)
        {
            close(fd);
        }
        delete [] filepath;
        cout<<"析构函数释放空间"<<this->name<<endl;
    }



    private:
    char *filepath;
    int fd;
    int size;
    string name;

};

int main()
{
    File open1("/mnt/d/Gitee/cpp/嵌入式学习/cpp/class/a.txt",100,"open1");




    return 0;
}