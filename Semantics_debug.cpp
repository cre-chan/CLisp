//
// Created by cre-chan on 19-12-8.
//


#include "stdafx.h"
#include "lexer.h"
#include "syntax.h"
#include "SymbolTable.hpp"
#include "symantics.h"


int main() {


    auto prelude = SymbolTable<FuncSignature>::NIL();
    auto global_names = SymbolTable<string>::NIL();

    unique_ptr<Expr> expr;

    while (cin>>expr) {

        auto[expr1, name_map, somthing]=symantic_check(std::move(expr), prelude, "");

        if (expr1)
            cout << *expr1 << endl;

//        cout<<*name_map<<endl;

//        prelude=name_map;
//        global_names=global_names+name_map;

    }

    return 0;
}