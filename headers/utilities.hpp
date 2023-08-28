#pragma once
#include "def.hpp"
#include <string>
#include <tuple>

/// @brief 按照little endian返回p[pos]表示的整数qword
/// @param  p 地址
/// @param pos 位置,按照byte记
/// @return 64位little endian整数
Qword readQword(Byte const* p,size_t pos=0);

/// @brief 返回p[pos]
/// @param p 地址
/// @param pos 位置,按照byte记
/// @return byte
Byte readByte(Byte const* p, size_t pos=0);

/// @brief 把p当做hex数组，返回pos位置的hex
/// @param p 地址
/// @param pos 位置，按照hex记
/// @return hex数
Byte readHex(Byte const* p,size_t pos=0);

/// @param p 位置
/// @return 连续的两个hex
std::tuple<Byte,Byte> readTwoHex(Byte const* p);

/// @brief 在p[pos]处写入一个Qword
void write(Byte* p,Qword word,size_t pos=0);

/// @brief 在p[pos]处写入一个Byte
void write(Byte* p,Byte word,size_t pos=0);

/// @brief 组合hex1的前4位和hex2的前四位
/// @return 一个字节
Byte combine(Byte hex1,Byte hex2=0);

/// @brief 将_str转换为dword储存在_num,0x开头为16进制,否则为十进制
/// @return 结果
Dword strToUll(std::string _str);