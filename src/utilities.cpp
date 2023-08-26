#include "utilities.hpp"

Qword peekQword(Byte const* p,size_t pos)
{
    return *reinterpret_cast<Qword const*>(p+pos);
}

Byte peekByte(Byte const* p, size_t pos)
{
    return p[pos];
}

Byte peekHex(Byte const *p, size_t pos)
{
    return p[pos/2]&(0xf<<(pos%2>>4));
}

std::tuple<Byte, Byte> peekTwoHex(Byte const *p)
{
    Byte byte=peekByte(p);
    return std::make_tuple(byte%0xf,byte&0x0f);
}
