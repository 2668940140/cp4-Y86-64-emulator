#include "utilities.hpp"

dword byteToDword(byte* p)
{
    return *reinterpret_cast<dword*>(p);
}