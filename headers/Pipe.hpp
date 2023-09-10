#pragma once
#include "def.hpp"
class Pipe : public StateDef
{
public:
Pipe(Byte* _Mem, size_t _MemSize, Byte* _iMem, size_t _iMemSize);

Byte run();

private:
enum {STALL,BUBBLE};

Pipe *const OB = this;

/// @brief 主内存头指针
Byte* Mem;

size_t MemSize;

/// @brief 指令内存头指针
Byte* iMem;

size_t iMemSize;

/// @brief 主寄存器
Qword RF[RFN]{};

/// @brief 条件位
bool CC[CCN]{};

/// @brief 主状态
Byte Stat;

/// @brief F阶段寄存器
struct FBuf
{
    Pipe* const OB = reinterpret_cast<Pipe*>((Byte*)this-offsetof(Pipe,F));
    Qword predPc{};
    bool Condition[2]{};
    void operator()();
}F;

struct FetchStage
{
    Pipe* const OB = reinterpret_cast<Pipe*>((Byte*)this-offsetof(Pipe,f));
    Byte stat{};
    Byte icode{};
    Byte ifun{};
    Byte rA{};
    Byte rB{};
    Qword valC{};
    Qword valP{};

    Qword predPc{};
    size_t pc{};

    void operator()();
}f;

/// @brief D阶段寄存器
struct DBuf
{
    Pipe* const OB = reinterpret_cast<Pipe*>((Byte*)this-offsetof(Pipe,D));
    Byte stat{};
    Byte icode{};
    Byte ifun{};
    Byte rA{};
    Byte rB{};
    Qword valC{};
    Qword valP{};
    bool Condition[2]{};
    void operator()();
}D;


struct DecodeStage
{
    Pipe* const OB = reinterpret_cast<Pipe*>((Byte*)this-offsetof(Pipe,d));
    Byte stat{};
    Byte icode{};
    Byte ifun{};
    Qword valA{};
    Qword valB{};
    Qword valC{};
    Byte dstE{};
    Byte dstM{};
    Byte srcA{};
    Byte srcB{};

    void operator()();

private:
    void forward(Qword& val,Byte src);
}d;

/// @brief E阶段寄存器
struct Ebuf
{
    Pipe* const OB = reinterpret_cast<Pipe*>((Byte*)this-offsetof(Pipe,E));
    Byte stat{};
    Byte icode{};
    Byte ifun{};
    Qword valA{};
    Qword valB{};
    Qword valC{};
    Byte dstE{};
    Byte dstM{};
    Byte srcA{};
    Byte srcB{};
    bool Condition[2]{};
    void operator()();
}E;

struct ExcuteStage
{
    Pipe* const OB = reinterpret_cast<Pipe*>((Byte*)this-offsetof(Pipe,e));
    Byte stat{};
    Byte icode{};
    bool Cnd{};
    
    Qword valE{};
    Qword valA{};

    Byte dstE{};
    Byte dstM{};

    bool* CC=OB->CC;

    void operator()();

    /// @brief //根据ifun CC 设置执行条件cnd
    void setCnd(Byte ifun);

    /// @brief 根据ifun对x,y计算并返回结果,同时设置CC,计算为valB ALU valA,
    Qword ALU(Qword valA, Qword valB,Byte ifun);
}e;

/// @brief M阶段寄存器
struct MBuf
{
    Pipe* const OB = reinterpret_cast<Pipe*>((Byte*)this-offsetof(Pipe,M));
    Byte stat{};
    Byte icode{};
    bool Cnd{};
    Qword valA{};
    Qword valE{};
    Byte dstE{};
    Byte dstM{};
    bool Condition[2]{};
    void operator()();
}M;

struct MemoryStage
{
    Pipe* const OB = reinterpret_cast<Pipe*>((Byte*)this-offsetof(Pipe,m));
    Byte stat{};
    Byte icode{};
    Qword valE{};
    Qword valM{};
    Byte dstE{};
    Byte dstM{};
    void operator()();
}m;

/// @brief W阶段寄存器
struct WBuf
{
    Pipe* const OB = reinterpret_cast<Pipe*>((Byte*)this-offsetof(Pipe,W));
    Byte stat{};
    Byte icode{};
    Qword valE{};
    Qword valM{};
    Byte dstE{};
    Byte dstM{};
    bool Condition[2]{};
    void operator()();
}W;

struct WriteBackStage
{
    Pipe* const OB = reinterpret_cast<Pipe*>((Byte*)this-offsetof(Pipe,w));
    void operator()();
}w;

/// @brief 检测3种hazard并设置对应Condition位
void hazardDetect();
};