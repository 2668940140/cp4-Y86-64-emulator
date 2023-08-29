cmake_minimum_required(VERSION 3.0.0)
enable_testing()

project(Seq LANGUAGES C CXX)
include_directories(emlator headers)
add_executable(Seq
    src/SeqTest.cpp
    src/Seq.cpp
    src/utilities.cpp
)
set_target_properties(Seq
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Seq"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Seq"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Seq"
)


project(assembler LANGUAGES C CXX)
include_directories(assembler headers)
add_executable(assembler
    src/Asm.cpp
    src/AsmTest.cpp
    src/utilities.cpp
)
set_target_properties(assembler
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assembler"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assembler"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assembler"
)


project(testcpp VERSION 0.1.0 LANGUAGES C CXX)
add_executable(testcpp
    tmp/test.cpp
)


project(assemblerSeqDoFibTest LANGUAGES C CXX)
add_compile_options(-g)
include_directories(assemblerSeqDoFibTest headers)
add_executable(assemblerSeqDoFibTest 
src/test1.cpp
src/Seq.cpp
src/utilities.cpp
src/Asm.cpp
)
set_target_properties(assemblerSeqDoFibTest
PROPERTIES
ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
)
add_test(NAME assemblerSeqDoFibTest COMMAND assemblerSeqDoFibTest 10)  #计算fib10，注意采用全递归，测试数太大可能爆栈