//
// Created by cre-chan on 19-12-23.
//

#include "stdafx.h"
#include "syntax.h"
#include "SymbolTable.hpp"
#include "symantics.h"
#include "codegen.h"



int main(){
    auto prelude=SymbolTable<FuncSignature>::NIL();
    unique_ptr<Expr> expr;

    while(cout<<">>>",cin>>expr){
        auto [result_expr,func_sigs,defs]=symantic_check(std::move(expr),prelude,"");
        prelude=prelude+func_sigs;


        if (result_expr) {
            cout << *result_expr << endl;

            auto exectubable = code_gen(std::move(result_expr));

            for (auto &i:exectubable) {
                switch (i.index()) {
                    case 0:
                        cout << get<0>(i) << ' ';
                        break;
                    case 1:
                        cout << get<1>(i) << ' ';
                        break;
                }
            }

            cout << endl;
        }
    }

    return 0;
}