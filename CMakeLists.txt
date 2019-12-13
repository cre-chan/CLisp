cmake_minimum_required(VERSION 3.14)

#add_subdirectory(GTest)

project(CLisp)

set(CMAKE_CXX_STANDARD 17)

include_directories(CLisp include)

add_executable(CLisp main.cpp lexer.cpp syntax.cpp syntaxer.cpp include/SymbolTable.hpp)


# 调试词法解析器用项目
project(Lexer)

include_directories(Lexer include)

add_executable(Lexer lexer_debug.cpp lexer.cpp )


#调试语法解析器
project(Syntaxer)

include_directories(Syntaxer include)

add_executable(Syntaxer syntax.cpp syntaxer_debug.cpp lexer.cpp syntaxer.cpp)


#符号表的调试
project(Symtable)

include_directories(Symtable include)

add_executable(Symtable sym_debug.cpp)


#语义分析的测试
project(Semantics)

include_directories(Semantics include)

add_executable(Semantics symantics.cpp Semantics_debug.cpp syntax.cpp syntaxer.cpp lexer.cpp)