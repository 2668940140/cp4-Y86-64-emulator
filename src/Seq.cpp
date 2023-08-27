#include "Seq.hpp"
#include "utilities.hpp"

void Seq::fetch()
{
    //TODO:检测是否合法
    std::tie(icode,ifun)=peekTwoHex(INS);

    switch(icode)
    {
        case CMOVXX:
        case RMMOVQ:
        case MRMOVQ:
        case OPQ:
        std::tie(rA,rB)=peekTwoHex(INS+1);
        break;

        case PUSHQ:
        case POPQ:
        rA=peekHex(INS+1);
        break;

        case IRMOVQ:
        rB=peekHex(INS+1,1);
        break;
    }

    switch(icode)
    {
        case IRMOVQ:
        case RMMOVQ:
        case MRMOVQ:
        valC=peekQword(INS,2);
        break;
        
        case JXX:
        case CALL:
        valC=peekQword(INS,1);
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

void Seq::decode()
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

void Seq::excute()
{


    switch(icode)
    {
        case HALT:
        //TODO
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
        valE=op(valB,valA);
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

        case FN::E:
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

Qword Seq::op(Qword x, Qword y)
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

Seq::_INS::operator const Byte *() const
{
    return object->iMem+object->pc;
}
