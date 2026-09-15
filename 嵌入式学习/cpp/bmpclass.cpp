#include "bmp.h"
#include <fstream>
#include <iostream>

Bmp::Bmp()
{
    m_pixel_buf = nullptr;
    m_loaded = false;
}

Bmp::~Bmp()
{
    release();
}

void Bmp::release()
{
    if (m_pixel_buf != nullptr)
    {
        delete[] m_pixel_buf;
        m_pixel_buf = nullptr;
    }
    m_loaded = false;
}

bool Bmp::load(const char* filePath)
{
    release(); // 加载新图前自动释放旧内存
    std::ifstream fp(filePath, std::ios::in | std::ios::binary);
    if (!fp.is_open())
    {
        std::cerr << "打开BMP文件失败：" << filePath << std::endl;
        return false;
    }

    // 读取文件头
    fp.read(reinterpret_cast<char*>(&m_hdr), sizeof(bitmap_header));
    if (m_hdr.type != 0x4D42) // 'BM'标记
    {
        std::cerr << "不是合法BMP文件" << std::endl;
        fp.close();
        return false;
    }

    // 读取信息头
    fp.read(reinterpret_cast<char*>(&m_info), sizeof(bitmap_info));

    // 限制：仅支持24bit无压缩BMP
    if (m_info.bit_count != 24 || m_info.compression != 0)
    {
        std::cerr << "仅支持24bit无压缩BMP图片" << std::endl;
        fp.close();
        return false;
    }

    int32_t w = m_info.width;
    int32_t h = m_info.height;
    // BMP每行4字节对齐
    int row_bytes = ((w * 3 + 3) / 4) * 4;
    int total_bytes = row_bytes * abs(h);

    m_pixel_buf = new uint8_t[total_bytes];
    // 跳到像素数据起始位置
    fp.seekg(m_hdr.offbits, std::ios::beg);
    fp.read(reinterpret_cast<char*>(m_pixel_buf), total_bytes);

    fp.close();
    m_loaded = true;
    std::cout << "BMP加载成功：宽=" << w << " 高=" << h << std::endl;
    return true;
}

int32_t Bmp::getWidth() const
{
    return m_loaded ? m_info.width : 0;
}

int32_t Bmp::getHeight() const
{
    return m_loaded ? m_info.height : 0;
}

int16_t Bmp::getBitCount() const
{
    return m_loaded ? m_info.bit_count : 0;
}

uint8_t* Bmp::getPixelData() const
{
    return m_pixel_buf;
}

const bitmap_header& Bmp::getFileHeader() const
{
    return m_hdr;
}

const bitmap_info& Bmp::getInfoHeader() const
{
    return m_info;
}
