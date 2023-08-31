#include "utilities.hpp"
#include <sstream>

namespace StrMapSpace
{
    std::unordered_map<std::string,Byte> MAP;
    std::unordered_map<std::string,size_t> SIZEOF;
    std::set<std::string> RARB_SET{"rrmovq","cmovle","cmovl","cmove","cmovne","cmovge","cmovg","addq","subq","andq","xorq"};
    std::set<std::string> VRB_SET{"irmovq"};
    std::set<std::string> RADRB_SET{"rmmovq"};
    std::set<std::string> DRBRA_SET{"mrmovq"};
    std::set<std::string> D_SET{"call","jmp","jle","jl","je","jne","jge","jg"};
    std::set<std::string> RA_SET{"pushq","popq"};
    std::set<std::string> NONE_SET{"halt","nop","ret"};
    std::set<std::string> REG_SET{"rax","rcx","rdx","rbx","rsp","rbp","rsi","rdi","r8","r9","r10","r11","r12","r13","r14"};
    std::set<std::string> INS_SET{"rrmovq","cmovle","cmovl","cmove","cmovne","cmovge","cmovg","addq","subq","andq","xorq","irmovq","rmmovq","mrmovq",
    "call","jmp","jle","jl","je","jne","jge","jg","pushq","popq","halt","nop","ret"};
}

namespace MCodeMapSpace
{
    std::unordered_map<Byte,size_t> SIZEOF;
}

struct __Init:StateDef
{
    __Init()
    {
        {
            using namespace StrMapSpace;
            using namespace Utl;
            MAP["halt"]=combine(HALT);
            MAP["nop"]=combine(NOP);
            MAP["rrmovq"]=combine(CMOVXX,FN::TRUE);
            MAP["cmovle"]=combine(CMOVXX,FN::LE);
            MAP["cmovl"]=combine(CMOVXX,FN::L);
            MAP["cmove"]=combine(CMOVXX,FN::EQ);
            MAP["cmovne"]=combine(CMOVXX,FN::NE);
            MAP["cmovge"]=combine(CMOVXX,FN::GE);
            MAP["cmovg"]=combine(CMOVXX,FN::G);
            MAP["irmovq"]=combine(IRMOVQ);
            MAP["rmmovq"]=combine(RMMOVQ);
            MAP["mrmovq"]=combine(MRMOVQ);
            MAP["addq"]=combine(OPQ,FN::ADDQ);
            MAP["subq"]=combine(OPQ,FN::SUBQ);
            MAP["andq"]=combine(OPQ,FN::ANDQ);
            MAP["xorq"]=combine(OPQ,FN::XORQ);
            MAP["jmp"]=combine(JXX,FN::TRUE);
            MAP["jle"]=combine(JXX,FN::LE);
            MAP["jl"]=combine(JXX,FN::L);
            MAP["je"]=combine(JXX,FN::EQ);
            MAP["jne"]=combine(JXX,FN::NE);
            MAP["jge"]=combine(JXX,FN::GE);
            MAP["jg"]=combine(JXX,FN::G);
            MAP["call"]=combine(CALL);
            MAP["ret"]=combine(RET);
            MAP["pushq"]=combine(PUSHQ);
            MAP["popq"]=combine(POPQ);
            //共27种

            //对于reg specifier是4bit的信息，需要之后combine
            MAP["rax"]=RAX;
            MAP["rcx"]=RCX;
            MAP["rdx"]=RDX;
            MAP["rbx"]=RBX;
            MAP["rsp"]=RSP;
            MAP["rbp"]=RBP;
            MAP["rsi"]=RSI;
            MAP["rdi"]=RDI;
            MAP["r8"]=R8;
            MAP["r9"]=R9;
            MAP["r10"]=R10;
            MAP["r11"]=R11;
            MAP["r12"]=R12;
            MAP["r13"]=R13;
            MAP["r14"]=R14;

            SIZEOF["halt"]=1;
            SIZEOF["nop"]=1;
            SIZEOF["rrmovq"]=2;
            SIZEOF["cmovle"]=2;
            SIZEOF["cmovl"]=2;
            SIZEOF["cmove"]=2;
            SIZEOF["cmovne"]=2;
            SIZEOF["cmovge"]=2;
            SIZEOF["cmovg"]=2;
            SIZEOF["irmovq"]=10;
            SIZEOF["rmmovq"]=10;
            SIZEOF["mrmovq"]=10;
            SIZEOF["addq"]=2;
            SIZEOF["subq"]=2;
            SIZEOF["andq"]=2;
            SIZEOF["xorq"]=2;
            SIZEOF["jmp"]=9;
            SIZEOF["jle"]=9;
            SIZEOF["jl"]=9;
            SIZEOF["je"]=9;
            SIZEOF["jne"]=9;
            SIZEOF["jge"]=9;
            SIZEOF["jg"]=9;
            SIZEOF["call"]=9;
            SIZEOF["ret"]=1;
            SIZEOF["pushq"]=2;
            SIZEOF["popq"]=2;
        }
        {
            using namespace MCodeMapSpace;
            auto & STRMAP = StrMapSpace::MAP;

            SIZEOF[STRMAP["halt"]]=1;
            SIZEOF[STRMAP["nop"]]=1;
            SIZEOF[STRMAP["rrmovq"]]=2;
            SIZEOF[STRMAP["cmovle"]]=2;
            SIZEOF[STRMAP["cmovl"]]=2;
            SIZEOF[STRMAP["cmove"]]=2;
            SIZEOF[STRMAP["cmovne"]]=2;
            SIZEOF[STRMAP["cmovge"]]=2;
            SIZEOF[STRMAP["cmovg"]]=2;
            SIZEOF[STRMAP["irmovq"]]=10;
            SIZEOF[STRMAP["rmmovq"]]=10;
            SIZEOF[STRMAP["mrmovq"]]=10;
            SIZEOF[STRMAP["addq"]]=2;
            SIZEOF[STRMAP["subq"]]=2;
            SIZEOF[STRMAP["andq"]]=2;
            SIZEOF[STRMAP["xorq"]]=2;
            SIZEOF[STRMAP["jmp"]]=9;
            SIZEOF[STRMAP["jle"]]=9;
            SIZEOF[STRMAP["jl"]]=9;
            SIZEOF[STRMAP["je"]]=9;
            SIZEOF[STRMAP["jne"]]=9;
            SIZEOF[STRMAP["jge"]]=9;
            SIZEOF[STRMAP["jg"]]=9;
            SIZEOF[STRMAP["call"]]=9;
            SIZEOF[STRMAP["ret"]]=1;
            SIZEOF[STRMAP["pushq"]]=2;
            SIZEOF[STRMAP["popq"]]=2;
        }
    }

}__init;

namespace Utl
{
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
        return (p[pos/2]&(0xf<<((pos+1)%2*4)))>>(pos+1)%2*4;
    }

    std::tuple<Byte, Byte> readTwoHex(Byte const *p)
    {
        Byte byte=readByte(p);
        return std::make_tuple((byte&0xf0)>>4,byte&0x0f);
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
        return (hex1&0xf)<<4|(hex2&0xf);
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

    std::string ullToHex(Dword _num)
    {
        static std::stringstream ss;
        ss.clear();ss.str("");
        ss<<"0x"<<std::hex<<_num;
        return ss.str();
    }

    /// @return _str是否是合法寄存器
    bool regQ(std::string const& _str)
    {
        using StrMapSpace::REG_SET;
        return REG_SET.find(_str)!=REG_SET.end();
    }
    bool insValidQ(Byte icode, Byte ifun)
    {
        switch(icode)
        {
            case StateDef::HALT:
            case StateDef::NOP:
            case StateDef::IRMOVQ:
            case StateDef::RMMOVQ:
            case StateDef::MRMOVQ:
            case StateDef::CALL:
            case StateDef::RET:
            case StateDef::PUSHQ:
            case StateDef::POPQ:
            return ifun==0;
            break;

            case StateDef::OPQ:
            return ifun<StateDef::FN::OPN;
            break;

            case StateDef::JXX:
            case StateDef::CMOVXX:
            return ifun<StateDef::FN::BRANCHN;
            break;
            
            default:
            return false;
        }
    }
};
