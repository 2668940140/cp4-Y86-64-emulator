#pragma once
#include "def.hpp"

// TODO: 异常处理

class Seq : public StateDef
{
public:
    /// @brief 提供如下内容以供构造
    /// @param instruction_mem_pointer 指令内存指针
    /// @param mem_pointer 内存指针
    /// @param mem_size 内存大小
    Seq(Byte *instruction_mem_pointer, Byte *mem_pointer, size_t mem_size)
    {
        iMem = instruction_mem_pointer;
        mem = mem_pointer;
        memSize = memSize;
    }

private:
    /// @brief iMEM+pc
    struct _INS
    {
        Seq const *object = reinterpret_cast<Seq const *>((Byte const *)(this) - offsetof(Seq, INS));
        operator const Byte *() const;
    } INS;

    Byte icode; // 指令类型byte
    Byte ifun;  // 指令功能说明byte
    Byte rA;    // 寄存器指定byte A
    Byte rB;    // 寄存器指定byte B
    Qword valC;
    size_t valP;
    Qword valA;
    Qword valB;
    Qword desE;
    Qword desM;
    Qword srcA;
    Qword secB;
    bool cnd; // 执行条件
    Qword valE;
    Qword valM;
    Byte stat=AOK;
    size_t pc = 0;
    size_t newPc;

    Qword RF[RFN];         // Register File
    bool CC[CCN]{1, 0, 0}; // Conditional Codes

    Byte *mem = nullptr;
    Byte *iMem = nullptr;
    size_t memSize = 0;

    size_t cycle = 0;

private:
    void fetchStage();

    void decodeStage();

    void excuteStage();

    void memoryStage();

    void writebackStage();

    void pcUpdateStage();

    /// @brief //根据ifun CC 设置执行条件cnd
    void setCnd();

    /// @brief 根据ifun对x,y计算并返回结果,同时设置CC,计算为X op Y,
    /// @param x valB
    /// @param y valA
    Qword op(Qword x, Qword y);

    /// @return 运行结束时的stat
    Byte run();
};
