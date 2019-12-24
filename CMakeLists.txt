cmake_minimum_required(VERSION 3.14)


set(CMAKE_CXX_STANDARD 17)


# 调试词法解析器用项目
project(Lexer)
include_directories(Lexer include)
add_executable(Lexer debug_repl/lexer_debug.cpp lexer.cpp clisp_execption.cpp)


#调试语法解析器
project(Parser)
include_directories(Parser include)
add_executable(Parser syntax.cpp debug_repl/syntaxer_debug.cpp lexer.cpp parser.cpp clisp_execption.cpp)


#符号表的调试
project(Symtable)
include_directories(Symtable include)
add_executable(Symtable debug_repl/sym_debug.cpp)


#语义分析的测试
project(Semantics)
include_directories(Semantics include)
add_executable(Semantics symantics.cpp debug_repl/Semantics_debug.cpp syntax.cpp parser.cpp lexer.cpp clisp_execption.cpp)


#代码生成
project(Codegen)
include_directories(Codegen include)
add_executable(Codegen debug_repl/codegen_debug.cpp lexer.cpp parser.cpp syntax.cpp clisp_execption.cpp symantics.cpp codegen.cpp)


#求值环境
project(CLisp)
include_directories(CLisp include)
add_executable(CLisp main.cpp lexer.cpp syntax.cpp parser.cpp clisp_execption.cpp symantics.cpp codegen.cpp runtime.cpp include/runtime.h)
