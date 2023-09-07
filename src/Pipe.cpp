#include "Pipe.hpp"
#include "utilities.hpp"
#include <cassert>
#include <cstring>
#include <tuple>
using std::tuple;
using std::tie;
using std::make_tuple;
using namespace Utl;
using namespace MCodeMapSpace;

#define F (OB->F)
#define f (OB->f)
#define D (OB->D)
#define d (OB->d)
#define E (OB->E)
#define e (OB->e)
#define M (OB->M)
#define m (OB->m)
#define W (OB->W)
#define w (OB->w)
#define Mem (OB->Mem)
#define MemSize (OB->MemSize)
#define iMem (OB->iMem)
#define iMemSize (OB->iMemSize)
#define RF (OB->RF)
#define Stat (OB->Stat)
#define Ins (iMem+pc)

void Pipe::FetchStage::operator()()
{
    //select pc
    if(M.icode==JXX&&!M.Cnd)
    {
        pc=M.valA;
        //Mispredicted branch
    }
    else if(W.icode==RET)
        pc=W.valM;
    else pc=F.predPc;

    if(pc>=iMemSize)
    {
        stat=ADR;
        return;
    }
    else tie(icode,ifun)=readTwoHex(iMem+pc);

    if(!insValidQ(icode,ifun))
    {
        stat=INS;
        return;
    }
    else if(pc+SIZEOF[combine(icode,ifun)]>iMemSize)
    {
        stat=ADR;
        return;
    }

    if(icode==HALT)
    {
        stat=HALT;
        return;
    }

    //read detail-reg
    switch(icode)
    {
        case CMOVXX:
        case RMMOVQ:
        case MRMOVQ:
        case OPQ:
        tie(rA,rB)=readTwoHex(Ins+1);
        if(rA>=RFN||rB>=RFN)
        {
            stat=INS;
            return;
        }
        break;

        case PUSHQ:
        case POPQ:
        tie(rA,rB)=readTwoHex(Ins+1);
        if(rA>=RFN||rB!=RNONE)
        {
            stat=INS;
            return;
        }
        break;

        case IRMOVQ:
        tie(rA,rB)=readTwoHex(Ins+1);
        if(rA!=RNONE||rB>=RFN)
        {
            stat=INS;
            return;
        }
        break;
    }


    //read detail-D/V
    switch(icode)
    {
        case IRMOVQ:
        case RMMOVQ:
        case MRMOVQ:
        valC=readQword(Ins,2);
        break;
        
        case JXX:
        case CALL:
        valC=readQword(Ins,1);
        break;
    }

    //write valP
    switch(icode)
    {
        case HALT:
        case NOP:
        case RET:
        valP=pc+1;
        break;
 
        case CMOVXX:
        case OPQ:
        case PUSHQ:
        case POPQ:
        valP=pc+2;
        break;

        case JXX:
        case CALL:
        valP=pc+9;
        break;

        case IRMOVQ:
        case RMMOVQ:
        case MRMOVQ:
        valP=pc+10;
        break;
    }
    
    predPc=(icode==JXX||icode==CALL)?valC:valP;
    
    return;
}

void Pipe::FBuf::operator()()
{
    if(Condition[BUBBLE])
    {
        assert(false); //本程序编写正确不会出现此情况
    }

    if(Condition[STALL])
    {
        predPc=NOP;
    }
    else
    {
        predPc=f.predPc;
    }
    memset(Condition,0,sizeof(Condition));
}

void Pipe::DBuf::operator()()
{
    if(Condition[STALL]) //双
    {
        stat=AOK;
        icode=NOP;
        ifun=FN::TRUE;
        rA=RNONE;
        rB=RNONE;
        valC=0;
        valP=0;
    }
    else if(Condition[BUBBLE])
    {
        //pass
    }
    else
    {
        stat=f.stat;
        icode=f.icode;
        ifun=f.ifun;
        rA=f.rA;
        rB=f.rB;
        valC=f.valC;
        valP=f.valP;
    }
    memset(Condition,0,sizeof(Condition));
}

void Pipe::DecodeStage::operator()()
{
    stat=D.stat;
    icode=D.icode;
    ifun=D.ifun;
    valC=D.valC;
    srcA=D.rA;srcB=D.rB;

    //储存这些信息为了之后可能的forward
    switch(icode)
    {
        case HALT:
        case NOP:
        case RMMOVQ:
        case JXX:
        dstE=RNONE;
        dstM=RNONE;
        break;

        case CMOVXX:
        case IRMOVQ:
        case OPQ:
        dstE=D.rB;
        dstM=RNONE;
        break;

        case PUSHQ:
        case CALL:
        case RET:
        dstE=RSP;
        dstM=RNONE;
        break;

        case MRMOVQ:
        dstM=D.rA;
        dstE=RNONE;
        break;

        case POPQ:
        dstM=D.rA;
        dstE=RSP;
        break;
    }

    forward(valA,srcA);
    forward(valB,srcB);
}

void Pipe::DecodeStage::forward(Qword &val, Byte src)
{
    //此处有说明，forward只需要在一个cycle的结束前完成即可
    //虽然使用了小写阶段的信息但是可以保证正确性
    //在线性模拟中把d放在e后

    if(icode==CALL||icode==JXX)
    {
        val=D.valP;
    }
    else if (src==RNONE)
    {
        return;
    }
    else if(src==e.dstE)
    {
        val=e.valE;
    }
    else if(src==M.dstM)
    {
        val=m.valM;
    }
    else if(src==M.dstE)
    {
        val=M.valE;
    }
    else if(src==W.dstM)
    {
        val=W.valM;
    }
    else if(src==W.dstE)
    {
        val=W.valE;
    }
    else
    {
        val=RF[src];
    }
}

void Pipe::Ebuf::operator()()
{
    if(Condition[STALL])
    {
        assert(false);//程序正确不会出现这种情况
    }
    if(Condition[BUBBLE])
    {
        //pass
    }
    else
    {
        stat=d.stat;
        icode=d.icode;
        ifun=d.ifun;
        valC=d.valC;
        valA=d.valA;
        valB=d.valB;
        dstE=d.dstE;
        dstM=d.dstM;
        srcA=d.srcA;
        srcB=d.srcB;
    }
    memset(Condition,0,sizeof(Condition));
}

void Pipe::ExcuteStage::operator()()
{
    stat=E.stat;
    icode=E.icode;
    valA=E.valA;

    switch(icode)
    {
        case HALT:
        case NOP:
        //pass
        break;

        case CMOVXX:
        valE=E.valA;
        setCnd(E.ifun);
        break;

        case IRMOVQ:
        valE=E.valC;
        break;

        case RMMOVQ:
        case MRMOVQ:
        valE=E.valB+E.valC;
        break;

        case OPQ:
        valE=ALU(E.valA,E.valB,E.ifun);
        break;

        case JXX:
        setCnd(E.ifun);
        break;

        case CALL:
        case PUSHQ:
        valE=E.valB-8;
        break;

        case RET:
        case POPQ:
        valE=E.valB+8;
        break;
    }

    dstE=(icode==JXX&&!Cnd)?RNONE:E.dstE;
    dstM=E.dstM;

    
}

void Pipe::ExcuteStage::setCnd(Byte ifun)
{
    //CMOVXX和JXX的条件是相同的,可以通用
    switch(ifun)
    {
        case FN::TRUE:
        Cnd=true;
        break;

        case FN::LE:
        Cnd=CC[SF]^CC[OF]|CC[ZF];
        break;

        case FN::L:
        Cnd=CC[SF]^CC[OF];
        break;

        case FN::EQ:
        Cnd=CC[ZF];
        break;

        case FN::NE:
        Cnd=~CC[ZF];
        break;

        case FN::GE:
        Cnd=~(CC[SF]^CC[OF]);
        break;

        case FN::G:
        Cnd=~(CC[SF]^CC[OF])&~CC[ZF];
        break;
    }
}

Qword Pipe::ExcuteStage::ALU(Qword valA, Qword valB, Byte ifun)
{
    Qword ret;
    switch(ifun)
    {
        case FN::ADDQ:
        ret=valB+valA;
        break;

        case FN::SUBQ:
        ret=valB-valA;
        break;

        case FN::ANDQ:
        ret=valB&valA;
        break;

        case FN::XORQ:
        ret=valB^valA;
        break;
    }

    CC[ZF]=!ret;
    CC[SF]=ret>>sizeof(Qword)*8-1;
    switch(ifun)
    {
        case FN::ADDQ:
        CC[OF]=ret<valB;
        break;
        case FN::SUBQ:
        CC[OF]=ret>valB;
        break;

        case FN::ANDQ:
        case FN::XORQ:
        CC[OF]=false;
        break;
    }
    
    return ret;
}

void Pipe::MBuf::operator()()
{
    if(Condition[STALL]||Condition[BUBBLE]) assert(false); //程序编写正确不会出现这种情况

    stat=e.stat;
    icode=e.icode;
    Cnd=e.icode;
    valE=e.valE;
    valA=e.valA;
    dstE=e.dstE;
    dstM=e.dstM;
}

void Pipe::MemoryStage::operator()()
{
    stat=M.stat;
    icode=M.icode;
    valE=M.valE;
    dstE=M.dstE;
    dstM=M.dstM;

    switch(icode)
    {
        case HALT:
        case NOP:
        case CMOVXX:
        case IRMOVQ:
        case OPQ:
        case JXX:
        //pass
        break;


        case RMMOVQ:
        case PUSHQ:
        case CALL:
        if(M.valE+8>MemSize)
        {
            stat=ADR;
            return;
        }
        write(Mem,M.valA,M.valE);
        break;

        case MRMOVQ:
        if(M.valE+8>MemSize)
        {
            stat=ADR;
            return;
        }
        valM=readQword(Mem,M.valE);
        break;

        case POPQ:
        case RET:
        if(M.valA+8>MemSize)
        {
            stat=ADR;
            return;
        }
        valM=readQword(Mem,M.valA);
        break;
    }
}

void Pipe::WBuf::operator()()
{
    if(Condition[STALL]||Condition[BUBBLE]) assert(false); //程序编写正确不会出现这种情况

    stat=m.stat;
    icode=m.icode;
    valE=m.valE;
    valM=m.valM;
    dstE=m.dstE;
    dstM=m.dstM;
}

void Pipe::WriteBackStage::operator()()
{
    Stat=(W.stat==BUB)?AOK:W.stat;
    
    if(W.dstE!=RNONE) RF[W.dstE]=W.valE;
    if(W.dstM!=RNONE) RF[W.dstM]=W.valM;
}

Pipe::Pipe(Byte *_Mem, size_t _MemSize, Byte *_iMem, size_t _iMemSize)
{
    Mem=_Mem;
    MemSize=_MemSize;
    iMem=_iMem;
    iMemSize=_iMemSize;
}


void Pipe::run()
{
    size_t cycle = 0;
    while (Stat==AOK&&f.stat==AOK&&d.stat==AOK&&e.stat==AOK&&m.stat==AOK)
    {
        cycle++;
    //以下5个buf阶段可并行
        F();
        D();
        E();
        M();
        W();

    //以下5个阶段时钟模拟下可并行真实时钟模拟可并行
        f();
        e(); //使用线性模拟并行，e要放在d前保证forward
        d();
        m();
        w();

        hazardDetect();
    }
    
}

void Pipe::hazardDetect()
{
    bool processingRet = (d.icode==RET||e.icode==RET||m.icode==RET);
    bool mispredictedBranch= (M.icode==JXX&&!M.Cnd);
    bool loadUseHazard = (E.icode==IRMOVQ||E.icode==POPQ)&&(E.dstM==d.srcA||E.dstM==d.srcB);
    
    F.Condition[STALL]=processingRet||loadUseHazard;
    D.Condition[BUBBLE]=processingRet||mispredictedBranch;
    D.Condition[STALL]=loadUseHazard;
    E.Condition[BUBBLE]=loadUseHazard||mispredictedBranch;
}
