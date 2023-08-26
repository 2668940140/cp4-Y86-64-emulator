#pragma once
#include "def.hpp"

//TODO: 异常处理

class Seq
{
public:
    /// @brief 提供如下内容以供构造
    /// @param instruction_mem_pointer 指令内存指针
    /// @param mem_pointer 内存指针
    /// @param mem_size 内存大小
    Seq(Byte* instruction_mem_pointer,Byte* mem_pointer,size_t mem_size)
    {
        iMem=instruction_mem_pointer;
        mem=mem_pointer;memSize=memSize;
    }

private:
    static constexpr size_t RFN = 15;
    enum {RAX,RCX,RDX,RBX,RSP,RBP,RSI,RDI,R8,R9,R10,R11,R12,R13,R14,NO_REGISTER};

    static constexpr size_t CCN = 3;
    enum {ZF,SF,OF};

    static constexpr size_t INSTRUCTIONN=12;
    enum {HALT,NOP,RRMOVQ,IRMOVQ,RMMOVQ,MRMOVQ,OPQ,JXX,CMOVXX=2,CALL=8,RET,PUSHQ,POPQ};

    /// @brief 充当类内namespace
    struct FN{
        FN()=delete;
        static constexpr size_t OPN = 4;
        enum {ADDQ,SUBQ,ANDQ,XORQ};

        static constexpr size_t BRANCHN = 7;
        enum {JMP,JLE,JL,JE,JNE,JGE,JG};

        static constexpr size_t MOVEN = 7;
        enum {RRMOVQ,CMOVLE,CMOVL,CMOVE,CMOVNE,CMOVGE,CMOVG};
    };

    /// @brief iMEM+pc
    struct _INS{
        Seq const* object=reinterpret_cast<Seq const*>((Byte const*)(this)-offsetof(Seq,INS));
        operator const Byte*() const;
    }
    INS; 

    Byte icode; //指令类型byte
    Byte ifun; //指令功能说明byte
    Byte rA; //寄存器指定byte A
    Byte rB; //寄存器指定byte B
    Qword valC;
    size_t valP;
    Qword valA;
    Qword valB;
    Qword desE;
    Qword desM;
    Qword srcA;
    Qword secB;
    Byte cnd;
    Qword valE;
    Qword valM;
    Qword stat;
    size_t pc=0;
    size_t newPc;
 
    Qword RF[RFN]; //Register File
    bool CC[CCN]; //Conditional Codes

    Byte* mem=nullptr;
    Byte* iMem=nullptr;
    size_t memSize=0;

private:
    void fetch();

    void decode();

    void excute();
};