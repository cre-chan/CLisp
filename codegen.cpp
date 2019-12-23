//
// Created by cre-chan on 19-12-23.
//


#include "stdafx.h"
#include "syntax.h"
#include "SymbolTable.hpp"
#include "symantics.h"
#include "codegen.h"


template<class T>
void merge(vector<T> &v1, vector<T> v2) {
    for (auto &t:v2) {
        v1.push_back(std::move(t));
    }
}

Commands code_gen(unique_ptr<Expr> inputExpr) {
    Commands ret;
    ret.emplace_back(Operator{Operator::lbr, "("});

    switch (inputExpr->getTag()) {

        case Expr::cond: {
            auto[cond, expr1, expr2]=move(get<Expr::if_caluse_t>((*(*inputExpr))));
            merge(ret, code_gen(std::move(cond)));
            ret.emplace_back(Operator{Operator::opcodes::branch, "if"});
            merge(ret, code_gen(move(expr1)));
            merge(ret, code_gen((move(expr2))));
            break;
        }
        case Expr::eq: {
            auto[oper, op1, op2]=move(get<Expr::binary_relation_t>(**inputExpr));
            merge(ret, code_gen(move(op1)));
            merge(ret, code_gen(move(op2)));
            ret.emplace_back(Operator{Operator::eq, "="});
            break;
        }
        case Expr::le: {
            auto[oper, op1, op2]=move(get<Expr::binary_relation_t>(**inputExpr));
            merge(ret, code_gen(move(op1)));
            merge(ret, code_gen(move(op2)));
            ret.emplace_back(Operator{Operator::le, "<="});
            break;
        }
        case Expr::ge: {
            auto[oper, op1, op2]=move(get<Expr::binary_relation_t>(**inputExpr));
            merge(ret, code_gen(move(op1)));
            merge(ret, code_gen(move(op2)));
            ret.emplace_back(Operator{Operator::ge, ">="});
            break;
        }
        case Expr::lt: {
            auto[oper, op1, op2]=move(get<Expr::binary_relation_t>(**inputExpr));
            merge(ret, code_gen(move(op1)));
            merge(ret, code_gen(move(op2)));
            ret.emplace_back(Operator{Operator::lt, "<"});
            break;
        }
        case Expr::gt: {
            auto[oper, op1, op2]=move(get<Expr::binary_relation_t>(**inputExpr));
            merge(ret, code_gen(move(op1)));
            merge(ret, code_gen(move(op2)));
            ret.emplace_back(Operator{Operator::gt, ">"});
            break;
        }
        case Expr::id: {
            auto[var_name]=move(get<Expr::id_expr_t>(**inputExpr));
            ret.pop_back();
            ret.emplace_back(Value{false, var_name});
            return move(ret);
        }
        case Expr::boolean: {
            auto[bvalue]=get<Expr::bool_expr_t>(**inputExpr);
            ret.pop_back();
            ret.emplace_back(Value{false, bvalue});
            return move(ret);
        }
        case Expr::interger: {
            auto[ivalue]=get<Expr::int_expr_t>(**inputExpr);
            ret.pop_back();
            ret.emplace_back(Value{false, ivalue});
            return move(ret);
        }
        case Expr::apply: {
            auto[plist]=move(get<Expr::apply_expr_t>(**inputExpr));

            for (auto cur = plist.rbegin(); cur != plist.rend(); cur++)
                merge(ret, code_gen(move(*cur)));


//            ret.emplace_back(Operator{Operator::call,"call"});
//            ret.emplace_back(Value{false,var_name});
            break;
        }
//        case Expr::def_func:
//            break;
        case Expr::plus: {
            auto[oper,oplist]=move(get<Expr::arith_expr_t>(**inputExpr));

            for (auto &i:oplist)
                merge(ret, code_gen(move(i)));

            ret.emplace_back(Operator{Operator::plus, "+"});

            break;
        }
        case Expr::minus:{
            auto[oper,oplist]=move(get<Expr::arith_expr_t>(**inputExpr));

            for (auto &i:oplist)
                merge(ret, code_gen(move(i)));

            ret.emplace_back(Operator{Operator::minus, "-"});

            break;
        }
        case Expr::times:{
            auto[oper,oplist]=move(get<Expr::arith_expr_t>(**inputExpr));

            for (auto &i:oplist)
                merge(ret, code_gen(move(i)));

            ret.emplace_back(Operator{Operator::mult, "*"});

            break;
        }
        case Expr::div:{
            auto[oper,oplist]=move(get<Expr::arith_expr_t>(**inputExpr));

            for (auto &i:oplist)
                merge(ret, code_gen(move(i)));

            ret.emplace_back(Operator{Operator::div, "/"});

            break;
        }
        case Expr::def_with_expr:{
            auto[def_part,expr]=move(get<Expr::define_with_expr_t>(**inputExpr));

            merge(ret,code_gen(move(def_part)));

            merge(ret,code_gen(move(expr)));

            break;
        }
        case Expr::def_variable:{
            auto[var_name,expr]=move(get<Expr::define_val_t>(**inputExpr));

            merge(ret,code_gen(move(expr)));

            ret.emplace_back(Operator{Operator::bind,var_name});

            break;
        }
    }

    ret.emplace_back(Operator{Operator::rbr, ")"});
    return move(ret);
}

ostream &operator<<(ostream &os, const Operator &anOperator) {
    if (anOperator.opcode==Operator::bind)
        os<<"bind "<<anOperator.name;
    else
        os << anOperator.name;
    return os;
}

ostream &operator<<(ostream &os, const Value &value) {
    switch (value.val.index()) {
        case 1:
            os << get<1>(value.val);
            break;
        case 2:
            os << get<2>(value.val);
            break;
        case 3:
            os << get<3>(value.val);
            break;
    }
    return os;
}

FuncDefsLow code_gen(FuncDefs defs) {
    FuncDefsLow ret;

    for(auto &def:defs){
        auto [name,defexpr]=move(def);
        ret.push_front(pair<GlobalName,Commands>(name,code_gen(move(defexpr))));
    }

    return ret;
}
