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

}

Seq::_INS::operator const Byte *() const
{
    return object->iMem+object->pc;
}
