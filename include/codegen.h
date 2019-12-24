//
// Created by cre-chan on 19-12-23.
//

#ifndef CODEGEN_CODEGEN_H
#define CODEGEN_CODEGEN_H

#include <ostream>

using variable_name=string;
using func_name=string;


struct Operator{
    enum opcodes{
        plus,
        minus,
        mult,
        div,
        lbr,
        rbr,
        branch,
        lt,
        le,
        gt,
        ge,
        eq,
        bind,
        identity
    }opcode;
    string name;

    friend ostream &operator<<(ostream &os, const Operator &anOperator);
};


struct Value{
    friend ostream &operator<<(ostream &os, const Value &value);
    bool callable;
    variant<monostate,bool,int,variable_name> val;
};

using Term=variant<Operator,Value>;
using Commands=vector<Term>;
using FuncDefsLow=forward_list<pair<GlobalName ,Commands >>;



Commands code_gen(unique_ptr<Expr>);
FuncDefsLow code_gen(FuncDefs);

#endif //CODEGEN_CODEGEN_H
