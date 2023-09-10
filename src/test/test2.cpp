#include "Pipe.hpp"
#include "Asm.hpp"
#include "utilities.hpp"
#include <sstream>
#include <fstream>

const char* src = R"(#this program calculate Fibonacci number 
irmovq $0xffff,%rsp
xorq %r10,%r10
mrmovq (%r10),%rdi
call function
xorq %r10,%r10
rmmovq %rax,0(%r10)
halt

function:
irmovq $0x0,%r10
irmovq $0x1,%r11  #here for the judge sentence following
subq %rdi,%r10
je ret1
subq %rdi,%r11
je ret1
irmovq $0x1,%r10
subq %r10,%rdi
pushq %rdi
call function
popq %rdi
pushq %rax
irmovq $0x1,%r10
subq %r10,%rdi
call function
popq %r10
addq %r10,%rax
ret

ret1:
irmovq $0x1,%rax #return 1
ret)";

#define SIZE 524288
Byte Mem[SIZE];
Byte Ins[SIZE];
Pipe pipe(Mem,SIZE,Ins,SIZE);

using namespace std;
using namespace Utl;

Qword function(Qword x)
{
    if(x==0) return 1;
    if(x==1) return 1;
    return function(x-1)+function(x-2);
}

int main(int argc, char** argv){
    Qword input = strtoull(argv[1],nullptr,0);
    stringstream ss(src);
    write(Mem,input);
    if(!Y86_64Assembler(Ins,SIZE,ss)) return -1;
    if(pipe.run()!=Pipe::HLT) return -1;
    Qword ans = readQword(Mem);
    if(ans!=function(input)) return -1;
    return 0;
}
