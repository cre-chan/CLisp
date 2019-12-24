#include "stdafx.h"
#include <lexer.h>
#include <syntax.h>
#include "SymbolTable.hpp"
#include "symantics.h"
#include "codegen.h"
#include "runtime.h"
#include "clisp_execption.h"




void print_exec(const Commands& executable){
    for (auto &i:executable) {
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



int main(){
    auto prelude=SymbolTable<FuncSignature>::NIL();
    auto runtimeSymbols=SymbolTableRT::NIL();

    unique_ptr<Expr> expr;

    try {
        while (cout << ">>>", cin >> expr) {
            //进行语义分析，并更新语义分析上下文
            auto[result_expr, func_sigs, defs]=symantic_check(std::move(expr), prelude, "");

//        cout<<*defs<<endl;

            prelude = prelude + func_sigs;



            //将函数定义与函数签名合并
            runtimeSymbols = runtimeSymbols + sig_to_runtime(code_gen(move(defs)));

            if (result_expr) {
                //如果当前表达式可执行，则尝试执行
//            cout << *result_expr << endl;

                auto executable = code_gen(std::move(result_expr));

//            print_exec(executable);


                //生成可执行的队列,并使用当前的运行上下文执行该表达式
                auto run_expr = CodeText(executable);
                try {
                    auto result = evaluate(runtimeSymbols, run_expr);
                    switch (result.index()) {
                        case 1:
                            cout << get<int>(result) << endl;
                            break;
                        case 2:
                            cout << (get<bool>(result) ? "true" : "false") << endl;
                            break;
                        case 0:
                            cout << "<Function object>" << endl;
                    }
                } catch (shared_ptr<SymbolTableRT> &modification) {
                    runtimeSymbols = modification;
                    continue;
                }


            }

        }
    }catch (NormalExit&){

    }

    return 0;
}