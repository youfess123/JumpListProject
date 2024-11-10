cmake_minimum_required(VERSION 3.29)
project(untitled1)

set(CMAKE_CXX_STANDARD 20)

add_library(untitled1 STATIC JumpList.cpp
        JumpList.h)
