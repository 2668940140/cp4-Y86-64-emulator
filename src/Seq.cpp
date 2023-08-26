#include "Seq.hpp"
#include "utilities.hpp"

dword Seq::peekMem(size_t pos) const
{
    return byteToDword(mem+pos);
}
byte Seq::peekIns() const
{
    return iMem[pc];
}