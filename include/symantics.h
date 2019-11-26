//
// Created by cre-chan on 19-11-25.
//

#ifndef SYMANTICS_SYMANTICS_H
#define SYMANTICS_SYMANTICS_H

/*
 * 语法分析阶段进行一定程度的类型检查
 * 在全局符号表增加函数符号，并使其对应与抽象语法树
 *
 * */


std::tuple<SymbolTable<string>,SymbolTable<Expr>>
symantic_check();

#endif //SYMANTICS_SYMANTICS_H
