#include "Asm.hpp"
#include <string>
#include <sstream>
#include <unordered_map>
#include <array>

std::unordered_map<std::string,Byte> INS_MAP;

struct Init:StateDef
{
    static bool initilizedQ;//static默认是0
    void operator()()
    {
        initilizedQ=true;
        INS_MAP["halt"]=combine(HALT);
        INS_MAP["nop"]=combine(NOP);
        INS_MAP["rrmovq"]=combine(CMOVXX,FN::TRUE);
        INS_MAP["cmovle"]=combine(CMOVXX,FN::LE);
        INS_MAP["cmovl"]=combine(CMOVXX,FN::L);
        INS_MAP["cmove"]=combine(CMOVXX,FN::E);
        INS_MAP["cmovne"]=combine(CMOVXX,FN::NE);
        INS_MAP["cmovge"]=combine(CMOVXX,FN::GE);
        INS_MAP["cmovg"]=combine(CMOVXX,FN::G);
        INS_MAP["irmovq"]=combine(IRMOVQ);
        INS_MAP["rmmovq"]=combine(RMMOVQ);
        INS_MAP["mrmovq"]=combine(MRMOVQ);
        INS_MAP["addq"]=combine(OPQ,FN::ADDQ);
        INS_MAP["subq"]=combine(OPQ,FN::SUBQ);
        INS_MAP["andq"]=combine(OPQ,FN::ANDQ);
        INS_MAP["xorq"]=combine(OPQ,FN::XORQ);
        INS_MAP["jmp"]=combine(JXX,FN::TRUE);
        INS_MAP["jle"]=combine(JXX,FN::LE);
        INS_MAP["jl"]=combine(JXX,FN::L);
        INS_MAP["je"]=combine(JXX,FN::E);
        INS_MAP["jne"]=combine(JXX,FN::NE);
        INS_MAP["jge"]=combine(JXX,FN::GE);
        INS_MAP["jg"]=combine(JXX,FN::G);
        INS_MAP["call"]=combine(CALL);
        INS_MAP["ret"]=combine(RET);
        INS_MAP["pushq"]=combine(PUSHQ);
        INS_MAP["popq"]=combine(POPQ);
    }
};

bool _Assembler::operator()(Byte *p, size_t size, std::istream &is)
{
    if(!Init::initilizedQ) Init();
    size_t cur=0;

    using std::string;
    using std::stringstream;
    string line,instruction;Qword immediate,dest;
    stringstream lineIs;

    while (std::getline(is,line)&&size-cur>=10)
    {
        lineIs.clear();lineIs.str(line);
        lineIs>>instruction;

        if(auto name=INS_MAP.find(instruction);name==INS_MAP.end()) return false;
        else write(p,name->second,cur); cur+=1;


    }

    return false;
}

_Assembler Y86_64Assembler;