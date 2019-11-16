cmake_minimum_required(VERSION 3.14)
project(CLisp)

set(CMAKE_CXX_STANDARD 17)

include_directories(CLisp include)

add_executable(CLisp main.cpp lexer.cpp syntax.cpp)


# 调试词法解析器用项目
project(Lexer)

include_directories(Lexer include)

add_executable(Lexer lexer_debug.cpp lexer.cpp )


#调试语法解析器
project(Syntaxer)

include_directories(Syntaxer include)

add_executable(Syntaxer syntax.cpp syntaxer_debug.cpp)