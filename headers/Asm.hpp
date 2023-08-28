#pragma once
#include "utilities.hpp"
#include "def.hpp"
#include <iostream>

class _Assembler: StateDef
{
    public:
    /// @brief 将按行分割的Y86-64文本指令转换为二进制写入p中
    /// @param p 要写入的地址
    /// @param size p的内存大小
    /// @param is 输入流
    /// @return 是否成功
    bool operator()(Byte* p,size_t size,std::istream& is);
}extern Y86_64Assembler;