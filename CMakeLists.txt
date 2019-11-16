cmake_minimum_required(VERSION 3.14)
project(CLisp)

set(CMAKE_CXX_STANDARD 17)

include_directories(CLisp include)

add_executable(CLisp main.cpp lexer.cpp include/lexer.h)

# 调试词法解析器用项目
project(Lexer)

include_directories(Lexer include)

add_executable(Lexer lexer_debug.cpp lexer.cpp include/lexer.h)