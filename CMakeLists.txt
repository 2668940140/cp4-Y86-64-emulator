cmake_minimum_required(VERSION 3.0.0)
project(emulator VERSION 0.1.0 LANGUAGES C CXX)

include_directories(emlator headers)

add_executable(emulator
src/main.cpp
src/Seq.cpp
src/utilities.cpp
)


