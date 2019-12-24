//
// Created by cre-chan on 19-12-24.
//

#ifndef CLISP_RUNTIME_H
#define CLISP_RUNTIME_H


struct Function{
    vector<string> arglist;
    Commands executable;
};

using SymbolTableRT=SymbolTable<variant<Function,int,bool>>;

struct CodeText{
    Commands ::const_iterator pos;
    explicit CodeText(const Commands&);

    const Term& front();

    void increment();
};

variant<Function,int,bool> evaluate(const shared_ptr<SymbolTableRT>&,CodeText& code);

shared_ptr<SymbolTableRT> merge_sig_with_exec(shared_ptr<SymbolTable<FuncSignature>> sigs,FuncDefsLow);

#endif //CLISP_RUNTIME_H
