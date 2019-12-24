//
// Created by cre-chan on 19-12-8.
//


#include "stdafx.h"
#include "clisp_utility.hpp"
#include "lexer.h"
#include "syntax.h"
#include "SymbolTable.hpp"
#include "symantics.h"


int main() {
    auto prelude = SymbolTable<FuncSignature>::NIL();

    unique_ptr<Expr> expr;

    while (cin>>expr) {

        auto[expr1, name_map, definitions]=symantic_check(std::move(expr), prelude, "");

        prelude=prelude+name_map;

        if (expr1)
            cout << *expr1 << endl;

        for (const auto &def:definitions) {
            cout << def.first << ':' << endl;
//            cout << *def.second << endl;
        }

        cout<<*prelude<<endl;
    }

    return 0;
}