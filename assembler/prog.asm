#this program calculate Fibonacci number 
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
ret