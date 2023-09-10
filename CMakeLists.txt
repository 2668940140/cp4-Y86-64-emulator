cmake_minimum_required(VERSION 3.0.0)
enable_testing()

project(testcpp VERSION 0.1.0 LANGUAGES C CXX)
add_executable(testcpp
    src/test/test.cpp
)



include_directories(assemblerSeqDoFibTest headers)
add_executable(assemblerSeqDoFibTest 
src/test/test1.cpp
src/Seq.cpp
src/utilities.cpp
src/Asm.cpp
)
add_test(NAME assemblerSeqDoFibTest COMMAND assemblerSeqDoFibTest 10)  #计算fib10，注意采用全递归，测试数太大可能爆栈

include_directories(assemblerPipeDoFibTest headers)
add_executable(assemblerPipeDoFibTest 
src/test/test2.cpp
src/Pipe.cpp
src/utilities.cpp
src/Asm.cpp
)

add_test(NAME assemblerPipeDoFibTest COMMAND assemblerPipeDoFibTest 10)  #计算fib10，注意采用全递归，测试数太大可能爆栈W