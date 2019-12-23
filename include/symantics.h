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


struct FuncSignature {
    string global_name;
    std::forward_list<string> argnames;
};

using ExprModified=Expr;
using ScopeModifier=const string&;
using GlobalName=string;
using FuncDefs=forward_list<pair<GlobalName ,unique_ptr<ExprModified >>>;

using ResultType=std::tuple<
        unique_ptr<ExprModified >,
        shared_ptr<SymbolTable<FuncSignature>>,
        FuncDefs
>;

//返回一个(Optional<Expr>,local_name_to_func_signature, global_name_to_modified_ast)
tuple<
        unique_ptr<ExprModified >,
        shared_ptr<SymbolTable<FuncSignature>>,
        FuncDefs
>symantic_check(unique_ptr<Expr>, const shared_ptr<SymbolTable<FuncSignature>> &, ScopeModifier);

#endif //SYMANTICS_SYMANTICS_H
