//
// Created by cre-chan on 19-12-24.
//
#include "stdafx.h"
#include "syntax.h"
#include "SymbolTable.hpp"
#include "symantics.h"
#include "codegen.h"
#include "runtime.h"

//在求值栈里使用的项
using ValTerm=variant<Function, int, bool>;
using NumOfTerms=int;

void skip_expr(CodeText &code) {
    int count = 0;//括号的差

    do {
        if (code.front().index() == 0 && get<Operator>(code.front()).opcode == Operator::lbr)
            count++;

        if (code.front().index() == 0 && get<Operator>(code.front()).opcode == Operator::rbr)
            count--;

        code.increment();
    } while (count != 0);
}

variant<Function, int, bool> evaluate(const shared_ptr<SymbolTableRT> &env, CodeText &code) {
    auto &head = code.front();

    //将程序中的项转换成值
    auto term_to_rt_term = [&env](const variant<monostate, bool, int, variable_name> &term) -> ValTerm {
        switch (term.index()) {
            case 1:
                return get<1>(term);
            case 2:
                return get<2>(term);
            case 3:
                return *env->find(get<3>(term));
        }
    };

    //如果是项的话
    if (head.index() == 1) {
        auto &term = get<1>(head);
        code.increment();
        return term_to_rt_term(term.val);
    } else {
        vector<ValTerm> ops;
        code.increment();
        //取出所有运算数直到遇到运算符,每次循环开始时ind都是下一个term在的位置
        //此处的move是为了过类型检查，并不会有内存上的影响

        auto myenv=env;
        do {
            //尝试对各项进行求值，若抛出异常，则更新求值环境
            try {
                ops.push_back(evaluate(myenv, code));
            }catch(shared_ptr<SymbolTableRT>& tbl){
                myenv=tbl;
            }
        } while (code.front().index() == 1 || get<Operator>(code.front()).opcode == Operator::lbr);

        //先执行计算，因为在左括号之后不可能紧跟一个运算符，因此进行计算是安全的

        //获得运算符并将pc指向运算符下一个符号，视情况有可能需要再次跳跃
        auto &op = get<Operator>(code.front());
        code.increment();


//        code.increment();

        switch (op.opcode) {

            case Operator::plus: {
                int cur = get<int>(ops.front());
                for (int i = 1; i < ops.size(); i++)
                    cur += get<int>(ops[i]);

                code.increment();
                return cur;
            }
            case Operator::minus: {
                int cur = get<int>(ops.front());
                for (int i = 1; i < ops.size(); i++)
                    cur -= get<int>(ops[i]);

                code.increment();
                return cur;
            }
            case Operator::mult: {
                int cur = get<int>(ops.front());
                for (int i = 1; i < ops.size(); i++)
                    cur *= get<int>(ops[i]);

                code.increment();
                return cur;
            }
            case Operator::div: {
                int cur = get<int>(ops.front());
                for (int i = 1; i < ops.size(); i++)
                    cur /= get<int>(ops[i]);

                //跳过右括号
                code.increment();
                return cur;
            }
            case Operator::lbr:
                throw exception();
//          此分支不可能,lbr
            case Operator::rbr: {
                //从队尾移出apply的函数
                Function func = get<Function>(move(ops.back()));
                ops.pop_back();

                auto subenv = env;


                for (int i = 0; i < func.arglist.size(); i++) {
                    subenv = subenv->insert(func.arglist[i], make_shared<ValTerm>(ops[i]));
                }

                auto exec_proc = CodeText{func.executable};
                return evaluate(subenv, exec_proc);
            }
            case Operator::branch: {
                bool cond = get<bool>(ops.front());

                if (!cond) {
                    skip_expr(code);
                    auto ret = evaluate(env, code);
                    code.increment();
                    return ret;
                } else {
                    auto ret = evaluate(env, code);
                    skip_expr(code);
                    code.increment();
                    return ret;
                }
            }
            case Operator::lt: {
                int expr1, expr2;
                expr1 = get<int>(ops[0]);
                expr2 = get<int>(ops[1]);
                code.increment();
                return expr1 < expr2;
            }
            case Operator::le: {
                int expr1, expr2;
                expr1 = get<int>(ops[0]);
                expr2 = get<int>(ops[1]);
                code.increment();
                return expr1 <= expr2;
            }
            case Operator::gt: {
                int expr1, expr2;
                expr1 = get<int>(ops[0]);
                expr2 = get<int>(ops[1]);
                code.increment();
                return expr1 > expr2;
            }
            case Operator::ge: {
                int expr1, expr2;
                expr1 = get<int>(ops[0]);
                expr2 = get<int>(ops[1]);
                code.increment();
                return expr1 >= expr2;
            }
            case Operator::eq: {
                int expr1, expr2;
                expr1 = get<int>(ops[0]);
                expr2 = get<int>(ops[1]);
                code.increment();
                return expr1 == expr2;
            }
            case Operator::bind: {
                //跳过下一个括号并抛出修改过的上下文
                code.increment();
                throw env->insert(op.name, make_shared<ValTerm>(ops.front()));

            }
            case Operator::identity:
                code.increment();
                return ops.front();
//          此分支被删除
        }

    }

}

shared_ptr<SymbolTableRT> merge_sig_with_exec(shared_ptr<SymbolTable<FuncSignature>> sigs, FuncDefsLow defs) {
    auto ret = SymbolTableRT::NIL();

    for (auto &[name, exec]:defs) {
        vector<string> arglist;
        for (auto arg:move(sigs->find(name)->argnames))
            arglist.push_back(move(arg));

        //往ret里插入函数定义
        ret = ret->insert(name, make_shared<ValTerm>(Function{move(arglist), move(exec)}));
    }

    return ret;
}

CodeText::CodeText(const Commands &temp) : pos(temp.cbegin()) {

}

const Term &CodeText::front() {
    return *(pos);
}

void CodeText::increment() {
    pos++;
}
