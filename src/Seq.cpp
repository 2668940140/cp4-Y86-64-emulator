#include "Seq.hpp"
#include "utilities.hpp"
using namespace Utl;
void Seq::fetchStage()
{
    //TODO:检测是否合法
    std::tie(icode,ifun)=readTwoHex(INS);

    switch(icode)
    {
        case CMOVXX:
        case RMMOVQ:
        case MRMOVQ:
        case OPQ:
        std::tie(rA,rB)=readTwoHex(INS+1);
        break;

        case PUSHQ:
        case POPQ:
        rA=readHex(INS+1);
        break;

        case IRMOVQ:
        rB=readHex(INS+1,1);
        break;
    }

    switch(icode)
    {
        case IRMOVQ:
        case RMMOVQ:
        case MRMOVQ:
        valC=readQword(INS,2);
        break;
        
        case JXX:
        case CALL:
        valC=readQword(INS,1);
        break;
    }

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
}

void Seq::decodeStage()
{
    switch(icode)
    {
        case CMOVXX:
        case RMMOVQ:
        case MRMOVQ:
        case OPQ:
        valA=RF[rA];
        valB=RF[rB];
        break;

        case IRMOVQ:
        valB=RF[rB];
        break;

        case PUSHQ:
        valA=RF[rA];
        valB=RF[RSP];
        break;

        case POPQ:
        case RET:
        valA=RF[RSP];
        valB=RF[RSP];
        break;

        case CALL:
        valB=RF[RSP];
        break;
    }
}

void Seq::excuteStage()
{
    switch(icode)
    {
        case HALT:
        stat=HLT;
        break;

        case NOP:
        //pass
        break;

        case CMOVXX:
        valE=valA;
        setCnd();
        break;

        case IRMOVQ:
        valE=valC;
        break;

        case RMMOVQ:
        case MRMOVQ:
        valE=valB+valC;
        break;

        case OPQ:
        valE=ALU(valB,valA);
        break;

        case JXX:
        setCnd();
        break;

        case CALL:
        case PUSHQ:
        valE=valB-8;
        break;

        case RET:
        case POPQ:
        valE=valB+8;
        break;
    }
}

void Seq::memoryStage()
{
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
        write(mem,valA,valE);
        break;

        case MRMOVQ:
        valM=readQword(mem,valE);
        break;

        case POPQ:
        case RET:
        valM=readQword(mem,valA);
        break;

        case CALL:
        write(mem,valP,valE);
        break;
    }
}

void Seq::writebackStage()
{
    switch(icode)
    {
        case HALT:
        case NOP:
        case RMMOVQ:
        case JXX:
        //pass
        break;

        case CMOVXX:
        if(cnd) RF[rB]=valE;
        break;

        case IRMOVQ:
        case OPQ:
        RF[rB]=valE;
        break;

        case PUSHQ:
        case CALL:
        case RET:
        RF[RSP]=valE;
        break;

        case MRMOVQ:
        RF[rA]=valM;
        break;

        case POPQ:
        RF[RSP]=valE;
        RF[rA]=valM;
        break;
    }
}

void Seq::pcUpdateStage()
{
    switch(icode)
    {
        case HALT:
        case NOP:
        case RMMOVQ:
        case CMOVXX:
        case IRMOVQ:
        case MRMOVQ:
        case OPQ:
        case PUSHQ:
        case POPQ:
        pc=valP;
        break;

        case CALL:
        pc=valC;
        break;

        case JXX:
        pc=cnd?valC:valP;
        break;

        case RET:
        pc=valM;
        break;
    }
}

void Seq::setCnd()
{
    //CMOVXX和JXX的条件是相同的,可以通用
    switch(ifun)
    {
        case FN::TRUE:
        cnd=true;
        break;

        case FN::LE:
        cnd=CC[SF]^CC[OF]|CC[ZF];
        break;

        case FN::L:
        cnd=CC[SF]^CC[OF];
        break;

        case FN::EQ:
        cnd=CC[ZF];
        break;

        case FN::NE:
        cnd=~CC[ZF];
        break;

        case FN::GE:
        cnd=~(CC[SF]^CC[OF]);
        break;

        case FN::G:
        cnd=~(CC[SF]^CC[OF])&~CC[ZF];
        break;
    }
}

Qword Seq::ALU(Qword x, Qword y)
{
    Qword ret;
    switch(ifun)
    {
        case FN::ADDQ:
        ret=x+y;
        break;

        case FN::SUBQ:
        ret=x-y;
        break;

        case FN::ANDQ:
        ret=x&y;
        break;

        case FN::XORQ:
        ret=x^y;
        break;
    }

    CC[ZF]=!ret;
    CC[SF]=ret>>sizeof(Qword)*8-1;
    switch(ifun)
    {
        case FN::ADDQ:
        CC[OF]=ret<x;
        break;
        case FN::SUBQ:
        CC[OF]=ret>x;
        break;

        case FN::ANDQ:
        case FN::XORQ:
        CC[OF]=false;
        break;
    }
    
    return ret;
}

Seq::Seq(Byte *instruction_mem_pointer, Byte *mem_pointer, size_t mem_size)
{
        iMem = instruction_mem_pointer;
        mem = mem_pointer;
        memSize = memSize;
}

Byte Seq::run()
{
    //TODO STAT
    while (stat==AOK)
    {
        fetchStage();
        cycle++;

        decodeStage();
        cycle++;

        excuteStage();
        cycle++;

        memoryStage();
        cycle++;

        writebackStage();
        cycle++;

        pcUpdateStage();
        cycle++;
    }
    return stat;
}

Seq::_INS::operator const Byte *() const
{
    return object->iMem+object->pc;
}
