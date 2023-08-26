#pragma once
#include "def.hpp"
#include <tuple>

/// @brief 按照little endian返回p[pos]表示的整数qword
/// @param  p 地址
/// @param pos 位置,按照byte记
/// @return 64位little endian整数
Qword peekQword(Byte const* p,size_t pos=0);

/// @brief 返回p[pos]
/// @param p 地址
/// @param pos 位置,按照byte记
/// @return byte
Byte peekByte(Byte const* p, size_t pos=0);

/// @brief 把p当做hex数组，返回pos位置的hex
/// @param p 地址
/// @param pos 位置，按照hex记
/// @return hex数
Byte peekHex(Byte const* p,size_t pos=0);

/// @param p 位置
/// @return 连续的两个hex
std::tuple<Byte,Byte> peekTwoHex(Byte const* p);