#include "Asm.hpp"
#include <regex>
#include <string>
#include <sstream>
#include <unordered_map>
#include <array>
#include <set>

std::unordered_map<std::string,Byte> MAP;
std::set<std::string> RARB_SET{"rrmovq","cmovle","cmovl","cmove","cmovne","cmovge","cmovg","addq","subq","andq","xorq"};
std::set<std::string> VRB_SET{"irmovq"};
std::set<std::string> RADRB_SET{"rmmovq"};
std::set<std::string> DRBRA_SET{"mrmovq"};
std::set<std::string> D_SET{"call","jmp","jle","jl","je","jne","jge","jg"};
std::set<std::string> RA_SET{"pushq","popq"};
std::set<std::string> NONE_SET{"halt","nop","ret"};
std::set<std::string> REG_SET{"rax","rcx","rdx","rbx","rsp","rbp","rsi","rdi","r8","r9","r10","r11","r12","r13","r14"};
struct Init:StateDef
{
    static bool initilizedQ;//static默认是0
    void operator()()
    {
        initilizedQ=true;
        //对于指令是1byte的信息
        MAP["halt"]=combine(HALT);
        MAP["nop"]=combine(NOP);
        MAP["rrmovq"]=combine(CMOVXX,FN::TRUE);
        MAP["cmovle"]=combine(CMOVXX,FN::LE);
        MAP["cmovl"]=combine(CMOVXX,FN::L);
        MAP["cmove"]=combine(CMOVXX,FN::E);
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
        MAP["je"]=combine(JXX,FN::E);
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
    }
};
 
/// @return _str是否是合法寄存器
bool regQ(std::string const& _str)
{
    return REG_SET.find(_str)!=REG_SET.end();
}

bool _Assembler::operator()(Byte *p, size_t size, std::istream &is)
{
    if(!Init::initilizedQ) Init();
    size_t cur=0;

    using std::string;
    using std::stringstream;
    string line,instruction,detail;Qword immediate,dest;
    stringstream lineIs;

    while (std::getline(is,line)&&size-cur>=10)
    {
        lineIs.clear();lineIs.str(line);
        lineIs>>instruction;

        if(auto name=MAP.find(instruction);!lineIs.good()||name==MAP.end()) return false;
        else write(p,name->second,cur); cur+=1;


        std::regex regex;std::smatch results;
        if(RARB_SET.find(instruction)!=RARB_SET.end())
        {
            if(!(lineIs>>detail)) return false;
            regex.assign(R"(%(\w{2,3}),%(\w{2,3}))");
            std::regex_match(detail,results,regex);
            if(results.size()!=3||!regQ(results[1])||!regQ(results[2])) return false; 
            write(p,combine(MAP[results[1]],MAP[results[2]]),cur);cur++;
        }
        else if(VRB_SET.find(instruction)!=VRB_SET.end())
        {
            if(!(lineIs>>detail)) return false;
            regex.assign(R"(\$(\w+),%(\w{2,3}))");
            std::regex_match(detail,results,regex);
            if(results.size()!=3||!regQ(results[2])) return false;
            immediate=strToUll(results[1]);
            write(p,combine(0xf,MAP[results[2]]),cur);cur++;
            write(p,immediate,cur);cur+=8;
        }
        else if(RADRB_SET.find(instruction)!=RADRB_SET.end())
        {
            if(!(lineIs>>detail)) return false;
            regex.assign(R"(%(\w{2,3}),(\w*)\(%(\w{2,3})\))");
            std::regex_match(detail,results,regex);
            if(results.size()!=4||!regQ(results[1])||!regQ(results[3])) return false;
            dest=strToUll(results[2]);
            write(p,combine(MAP[results[1]],MAP[results[3]]),cur);cur++;
            write(p,dest,cur);cur+=8;
        }
        else if(DRBRA_SET.find(instruction)!=DRBRA_SET.end())
        {
            if(!(lineIs>>detail)) return false;
            regex.assign(R"((\w*)\(%(\w{2,3})\),%(\w{2,3}))");
            std::regex_match(detail,results,regex);
            if(results.size()!=4||!regQ(results[2])||!regQ(results[3])) return false;
            dest=strToUll(results[1]);
            write(p,combine(MAP[results[3],results[2]]),cur); cur++;
            write(p,dest,cur); cur+=8;
        }
        else if(D_SET.find(instruction)!=D_SET.end())
        {
            if(!(lineIs>>detail)) return false;
            dest=strToUll(detail);
            write(p,dest,cur); cur+=8;
        }
        else if(RA_SET.find(instruction)!=RA_SET.end())
        {
            if(!(lineIs>>detail)) return false;
            regex.assign(R"(%(\w{2,3}))");
            std::regex_match(detail,results,regex);
            if(results.size()!=2||!regQ(results[1])) return false;
            write(p,combine(MAP[results[1]],0xf),cur);cur++;
        }
        else if(NONE_SET.find(instruction)!=NONE_SET.end())
        {
            //pass
        }
    }
    return false;
}

_Assembler Y86_64Assembler;