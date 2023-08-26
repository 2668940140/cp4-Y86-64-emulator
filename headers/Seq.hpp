#include "def.hpp"

class Seq
{

public:
    Seq(byte* instruction_mem_pointer,byte* mem_pointer,size_t mem_size)
    {
        iMem=instruction_mem_pointer;
        mem=mem_pointer;memSize=memSize;
    }

private:
    static constexpr int RFN = 15;
    enum {RAX,RCX,RDX,RBX,RSP,RBP,RSI,RDI,R8,R9,R10,R11,R12,R13,R14,NO_REGISTER};

    static constexpr int CCN = 3;
    enum {ZF,SF,OF};

    static constexpr int INSTRUCTIONN=12;
    enum {HALT,NOP,RRMOVQ,IRMOVQ,RMMOVQ,MRMOVQ,OPQ,JXX,CMOVXX,CALL,RET,PUSHQ,POPQ};

    byte icode; //指令类型byte
    byte ifun; //指令功能说明byte
    byte rA; //寄存器指定byte A
    byte rB; //寄存器指定byte B
    dword valC;
    dword valP;
    dword valA;
    dword valB;
    dword desE;
    dword desM;
    dword srcA;
    dword secB;
    byte cnd;
    dword valE;
    dword valM;
    dword stat;
    int pc=0;
    int newPc;

    dword RF[RFN]; //Register File
    bool CC[CCN]; //Conditional Codes

    byte* mem=nullptr;
    byte* iMem=nullptr;
    size_t memSize=0;

private:
    /// @brief 查看mem的一个dword
    /// @param pos 从pos查看8个byte 
    /// @return 这8个byte组成的dword，little endian
    dword peekMem(size_t pos) const;

    /// @brief 查看下一个iCode
    /// @return 下一个iCode
    byte peekIns() const;

    
};