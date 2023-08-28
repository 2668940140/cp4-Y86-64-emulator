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

set_target_properties(assembler
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assembler"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assembler"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assembler"
)

set_target_properties(Seq
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Seq"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Seq"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Seq"
)