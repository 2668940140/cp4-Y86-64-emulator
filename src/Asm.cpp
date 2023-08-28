#include "Asm.hpp"
#include <regex>
#include <string>
#include <sstream>
#include <unordered_map>
#include <array>
#include <set>
#include <vector>
#include <cstring>

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


struct Instruction
{
    std::string ins;
    std::string detail;
    size_t offset=0;
};


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
};
 

/// @return _str是否是合法寄存器
bool regQ(std::string const& _str)
{
    return REG_SET.find(_str)!=REG_SET.end();
}

/// @brief 清除str中#开头的comment
void cleanComment(std::string& str)
{
    std::regex reg("#.*");
    str=regex_replace(str,reg,"");
}


/// @return label是否合法，规则同c++变量名
bool labelAvailableQ(std::string const& label)
{
    if(label.empty()) return false;
    if(isdigit(label[0])) return false;
    for(auto ch : label)
    {
        if(!(isdigit(ch)||isalpha(ch))) return false;
    }
    return true;
}

/// @brief 读取所有行
/// @return size_t(-1)表示失败,其他表示指令最后的offset
size_t parseLines(std::istream& _is,std::vector<Instruction> & lines)
{
    using std::string;
    using std::vector;
    using std::smatch;
    using std::regex_match;
    using std::map;
    using std::set;
    using std::stringstream;
    lines.clear();

    map<string,size_t> labelMap; //label->offset
    set<string> labels; //纯label字符串

    string content,line;
    while(getline(_is,line)) content+=line+"\n";

    cleanComment(content);
    stringstream ss(content);

    size_t offset=0;
    while (ss>>line)
    {
        if(line.back()==':')
        {
            string label=line.substr(0,line.size()-1);
            if(!labelAvailableQ(label)) return -1; //label不符合规则
            if(labels.find(label)!=labels.end()) return -1; //label定义重复

            labels.insert(label);
            labelMap[label]=offset;
        }
        else
        {
            if(INS_SET.find(line)==INS_SET.end()) return -1;
            Instruction newIns{line,"",offset};
            if(NONE_SET.find(line)==NONE_SET.end())
            {
                if(!(ss>>line)) return -1;
                newIns.detail=line;
            }
            offset+=SIZEOF[newIns.ins];
            lines.emplace_back(newIns);
        }
    }
    for(auto& ins:lines)
    {
        if(labels.find(ins.detail)!=labels.end())
        {
            ins.detail=ullToHex(labelMap[ins.detail]);
        }
    }
    return offset;
}
bool Init::initilizedQ;


/// @brief 根据一行写指令，不能有label
/// @param _line 这一行
/// @param _p 要写指令的内存
/// @return 是否成功
bool writeOneLine(Instruction const& _instruction,Byte* _p)
{
    static size_t cur;
    static std::string instruction,detail;
    static Qword immediate,dest;

    cur=_instruction.offset;instruction=_instruction.ins;detail=_instruction.detail;

    write(_p, MAP[instruction], cur); cur++;
    std::regex regex;
    std::smatch results;
    if (RARB_SET.find(instruction) != RARB_SET.end())
    {
        regex.assign(R"(%(\w{2,3}),%(\w{2,3}))");
        std::regex_match(detail, results, regex);
        if (results.size() != 3 || !regQ(results[1]) || !regQ(results[2]))
            return false;
        write(_p, combine(MAP[results[1]], MAP[results[2]]), cur);
        cur++;
    }
    else if (VRB_SET.find(instruction) != VRB_SET.end())
    {
        regex.assign(R"(\$(\w+),%(\w{2,3}))");
        std::regex_match(detail, results, regex);
        if (results.size() != 3 || !regQ(results[2]))
            return false;
        immediate = strToUll(results[1]);
        write(_p, combine(0xf, MAP[results[2]]), cur);
        cur++;
        write(_p, immediate, cur);
        cur += 8;
    }
    else if (RADRB_SET.find(instruction) != RADRB_SET.end())
    {
        regex.assign(R"(%(\w{2,3}),(\w*)\(%(\w{2,3})\))");
        std::regex_match(detail, results, regex);
        if (results.size() != 4 || !regQ(results[1]) || !regQ(results[3]))
            return false;
        dest = strToUll(results[2]);
        write(_p, combine(MAP[results[1]], MAP[results[3]]), cur);
        cur++;
        write(_p, dest, cur);
        cur += 8;
    }
    else if (DRBRA_SET.find(instruction) != DRBRA_SET.end())
    {
        regex.assign(R"((\w*)\(%(\w{2,3})\),%(\w{2,3}))");
        std::regex_match(detail, results, regex);
        if (results.size() != 4 || !regQ(results[2]) || !regQ(results[3]))
            return false;
        dest = strToUll(results[1]);
        write(_p, combine(MAP[results[3]], MAP[results[2]]), cur);
        cur++;
        write(_p, dest, cur);
        cur += 8;
    }
    else if (D_SET.find(instruction) != D_SET.end())
    {
        dest = strToUll(detail);
        write(_p, dest, cur);
        cur += 8;
    }
    else if (RA_SET.find(instruction) != RA_SET.end())
    {
        regex.assign(R"(%(\w{2,3}))");
        std::regex_match(detail, results, regex);
        if (results.size() != 2 || !regQ(results[1]))
            return false;
        write(_p, combine(MAP[results[1]], 0xf), cur);
        cur++;
    }
    else if (NONE_SET.find(instruction) != NONE_SET.end())
    {
        // pass
    }
    else
    {
        return false;
    }
    return true;
}

bool _Assembler::operator()(Byte *p, size_t size, std::istream &is)
{
    if(!Init::initilizedQ) Init()();

    std::vector<Instruction> lines;
    if(parseLines(is,lines)>=size) return false;
    for(auto const& line:lines)
    {
        if(!writeOneLine(line,p))
            return false;
    }
    return true;
}

_Assembler Y86_64Assembler;