# 总论
本库的目的是完成一个模拟的处理器类，采用CSAPP中Y86-64实现，仅体现处理器工作流程作为学习使用而不进行电路模拟

# 程序员可见状态
这里是Y86-64 instructions中暗示的内部架构，不一定代表内部实现。

1. 15个寄存器 RF:Program Registers(忽略了标准架构中的%r15来简化指令设计)

| | |
|--|--|--|--|--|--|--|--|
|%rax|%rcx|%rdx|%rbx|%rsp|%rbp|%rsi|%rdi|
|%r8|%r9|%r10|%r11|%r12|%r13|%r14|
2. CC Condition Codes 3个条件位

| | |
|--|--|--|
|ZF|SF|OF|

3. Stat: Program status 编码在下面，可以用1byte保存
4. DMEM:Memory

# Y86-64 指令集和编码
![[Pasted image 20230825174224.png]]
使用1~10 byte编码

可变byte如下:
![[Pasted image 20230825175044.png]]
![[Pasted image 20230825175057.png]]
![[Pasted image 20230825175329.png]]

# SEQ架构
按顺序执行，一个cycle执行下面的1个stage
1. Fetch:
从mem中PC地址读取指令;
解析指令并储存:
rA,rB   寄存器编码
valC   常量;
计算valP要更新的IP;

2. Decode:
根据rA,rB读取valA,valB;

3. Execute:
compute valE(result);
set conditional code;
conditional op(jump/mov etc.);

4. Memory:
wirte mem/read mem to val M;

5. PC update
![[Pasted image 20230826153238.png]]
