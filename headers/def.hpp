#pragma once
#include <cinttypes>
#include <cstddef>
typedef uint8_t Byte;
typedef uint16_t Word;
typedef uint32_t Dword;
typedef uint64_t Qword;

struct StateDef
{
    static constexpr size_t RFN = 15;
    enum {RAX,RCX,RDX,RBX,RSP,RBP,RSI,RDI,R8,R9,R10,R11,R12,R13,R14,RNONE};

    static constexpr size_t CCN = 3;
    enum {ZF,SF,OF};

    static constexpr size_t INSTRUCTIONN=12;
    enum {HALT,NOP,RRMOVQ,IRMOVQ,RMMOVQ,MRMOVQ,OPQ,JXX,CMOVXX=2,CALL=8,RET,PUSHQ,POPQ};

    /// @brief
    static constexpr size_t STATN=5;
    enum {AOK,HLT,ADR,INS,BUB};

    struct FN{
        static constexpr size_t OPN = 4;
        enum {ADDQ,SUBQ,ANDQ,XORQ};

        static constexpr size_t BRANCHN = 7;
        enum {TRUE,LE,L,EQ,NE,GE,G};
    };
};