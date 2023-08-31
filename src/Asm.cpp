#include "Asm.hpp"
#include "utilities.hpp"
#include <regex>
#include <string>
#include <sstream>
#include <unordered_map>
#include <array>
#include <set>
#include <vector>
#include <cstring>

using namespace StrMapSpace;
using namespace Utl;
struct Instruction
{
    std::string ins;
    std::string detail;
    size_t offset=0;
};

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