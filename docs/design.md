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

5. Write back:
write register file
6. PC update
![[Pasted image 20230826153238.png]]



# Pipeline架构:
并行的思想，但是要解决可能发生的冲突。
分划尽量平均的stage自然具有或者处理后具有并行无冲突的性质。

## 冲突:
### 不均衡的划分:
单Instrution耗时是T，分成了可并行的n个stage，用时是$t_1,t_2...t_n$
$\sum_{i=1,2...n}t_i=T$
latency=$n\cdot max(t_i)$
throughout=1/latency*n=1/$max(t_i)$

n固定时平均分latency最小,throughout最大

### 不共享状态:
pipeline每一个stage代表一个状态，vals不共享，需要传播vals耗时

### 依赖:不具有并行性
1. 数据依赖:后面的指令用到了前面修改的数据
2. 控制依赖:后面指令的条件依赖于前面指令执行


## SEQ+
传播的pc，在cycle开头计算，平衡stage的delay
![[Pasted image 20230829165246.png]]

## PIPE-:
![[Pasted image 20230829165624.png]]
f:表示预测值
D:取回和解码阶段过渡的缓存
E:解码和执行阶段过渡的缓存
M:执行和写内存阶段过渡的缓存，含条件变量
W:写内存和写寄存器阶段过渡的缓存

优化:过程中valP和valA在D后用同一"valE"储存

pc预测:为了循环起来，需要预测，本书采取简单的下一个pc=valP的预测方式(ret 不预测，直接stall)
### stall解决依赖问题:
将可能受影响的stall在fetch，直到可能影响它的通过write back
做法：给excute一个动态生成的nop(bubble)
固定fetch的pc

## PIPE:
 forward解决依赖问题:提前传过去修改后的数（不能解决还没有读取/算出来的数的情况）
需要增加额外的数据连接和控制逻辑

stall+forward解决还没有读取/算出来的数的情况

control hazard:跑了预测错误的指令
指令最早在excute阶段才能修改状态(CC),发现跑错了在excute插入bubble直到错误指令消失

![[Pasted image 20230830124029.png]]
# 异常处理
 内部异常:
1. halt
2. invalid 指令
3. invalid 数据访问

外部异常:
signals

在更完整的设计中应调用异常处理程序