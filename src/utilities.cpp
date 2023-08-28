#include "utilities.hpp"

Qword readQword(Byte const* p,size_t pos)
{
    return *reinterpret_cast<Qword const*>(p+pos);
}

Byte readByte(Byte const* p, size_t pos)
{
    return p[pos];
}

Byte readHex(Byte const *p, size_t pos)
{
    return p[pos/2]&(0xf<<(pos%2>>4));
}

std::tuple<Byte, Byte> readTwoHex(Byte const *p)
{
    Byte byte=readByte(p);
    return std::make_tuple(byte%0xf,byte&0x0f);
}

void write(Byte *p, Qword word,size_t pos)
{
    *reinterpret_cast<Qword*>(p+pos)=word;
}

void write(Byte *p, Byte word,size_t pos)
{
    p[pos]=word;
}

Byte combine(Byte hex1, Byte hex2)
{
    return (hex1&0xf)|(hex2&0xf)<<4;
}

Dword strToUll(std::string _str)
{
    // TODO:检测数字是否dword存不下,或者格式不对
    if (_str.empty())
        return 0;
    else if (_str.size() >= 2 && _str.substr(0, 2) == "0x")
    {
        return std::stoull(_str, 0, 16);
    }
    else
    {
        return std::stoull(_str, 0, 10);
    }
}