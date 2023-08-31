#pragma once
#include "def.hpp"
#include <string>
#include <tuple>
#include <unordered_map>
#include <set>

/// @brief 字符串映射空间，主要是assembler用
namespace StrMapSpace
{
    /// @brief 将指令字符串映射到机器码
    extern std::unordered_map<std::string,Byte> MAP;

    /// @brief 将指令字符串映射到它所占的字节数
    extern std::unordered_map<std::string,size_t> SIZEOF;

    /// @brief 形式是 rA,rB的指令字符串
    extern std::set<std::string> RARB_SET;

    /// @brief 形式是 V,rB的指令字符串
    extern std::set<std::string> VRB_SET;

    /// @brief 形式是 rA,D(rB)的指令字符串
    extern std::set<std::string> RADRB_SET;

    /// @brief 形式是 D(rB),rA的指令字符串
    extern std::set<std::string> DRBRA_SET;

    /// @brief 形式是 D的指令字符串
    extern std::set<std::string> D_SET;

    /// @brief 形式是 rA的指令字符串
    extern std::set<std::string> RA_SET;

    /// @brief 没有后续字节的指令字符串
    extern std::set<std::string> NONE_SET;

    /// @brief 寄存器字符串集
    extern std::set<std::string> REG_SET;

    /// @brief 指令字符串集
    extern std::set<std::string> INS_SET;
}

/// @brief 机器码映射空间
namespace MCodeMapSpace
{
    /// @brief 将指令机器码映射到它所占的字节数
    extern std::unordered_map<Byte,size_t> SIZEOF;
}

namespace Utl
{
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

    /// @brief 将一个ull转换为0x开头的小写hex
    /// @return 结果
    std::string ullToHex(Dword _num);

    /// @return _str是否是合法寄存器
    bool regQ(std::string const& _str);

    /// @brief 判断icode和ifun组合是否合法
    bool insValidQ(Byte icode, Byte ifun);
}