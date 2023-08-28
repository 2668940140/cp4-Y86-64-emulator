cmake_minimum_required(VERSION 3.0.0)
project(Seq VERSION 0.1.0 LANGUAGES C CXX)
include_directories(emlator headers)

add_executable(Seq
    src/SeqTest.cpp
    src/Seq.cpp
    src/utilities.cpp
)

project(assembler VERSION 0.1.0 LANGUAGES C CXX)
include_directories(assembler headers)
add_executable(assembler
    src/Asm.cpp
    src/AsmTest.cpp
    src/utilities.cpp
)

project(testcpp VERSION 0.1.0 LANGUAGES C CXX)
add_executable(testcpp
    tmp/test.cpp
)