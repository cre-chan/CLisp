#include <memory>

//
// Created by cre-chan on 19-11-25.
//

#include "stdafx.h"
#include "syntax.h"
#include "SymbolTable.hpp"
#include "symantics.h"

ResultType
check_relation(unique_ptr<Expr> expr, const shared_ptr<SymbolTable<FuncSignature>> &env,
               const std::function<unique_ptr<Expr>(unique_ptr<Expr>, unique_ptr<Expr>)> &retType,
               const string &name_modifier) {
    auto &[t, op1, op2] =std::get<Expr::binary_relation>(*(*expr));
    //不拓展模块，故修饰名不变
    auto[expr1, a, expr1_symbols]=symantic_check(std::move(op1), env, name_modifier);
    auto[expr2, b, expr2_symbols]=symantic_check(std::move(op2), env, name_modifier);

    return ResultType(
            retType(std::move(expr1), std::move(expr2)),
            env,
            std::move(expr1_symbols) + std::move(expr2_symbols)
    );
}

ResultType
check_cond(unique_ptr<Expr> expr, const shared_ptr<SymbolTable<FuncSignature>> &env, const string &name_modifier) {
    auto &[condition, op1, op2] = std::get<Expr::if_caluse>(*(*expr));
    auto[predicate, a, pred_symbols] = symantic_check(std::move(condition), env, name_modifier);
    auto[expr1, b, expr1_symbols] = symantic_check(std::move(op1), env, name_modifier);
    auto[expr2, c, expr2_symbols] = symantic_check(std::move(op2), env, name_modifier);


    return ResultType(
            Expr::IfExpr(std::move(predicate), std::move(expr1), std::move(expr2)), env,
            std::move(pred_symbols) + std::move(expr1_symbols) + std::move(expr2_symbols));
}


ResultType
check_def(unique_ptr<Expr> expr, const shared_ptr<SymbolTable<FuncSignature>> &env, const string &name_modifier) {
    auto[arglist, body]=std::move(std::get<Expr::define_func>(*(*expr)));

    auto func_decl_head = std::make_move_iterator(arglist.begin());
    auto fun_decl_tail = std::make_move_iterator(arglist.end());

    auto[local_name]=std::move(*(func_decl_head++));
    auto global_name = name_modifier + local_name;

    std::forward_list<string> arg_names;

    for (auto cur = arg_names.before_begin(); func_decl_head != fun_decl_tail; func_decl_head++, cur++) {
        auto[arg_name]=*func_decl_head;
        arg_names.insert_after(cur, std::move(arg_name));
    }

    //将局部名字到全局函数签名的映射加入环境
    auto func_env = env->insert(local_name, std::make_shared<FuncSignature>(
            FuncSignature{global_name, std::move(arg_names)}));

    auto[expr1, a, fundefs]=symantic_check(std::move(body), func_env, name_modifier + "@" + local_name);

    fundefs.push_front(pair<GlobalName ,unique_ptr<ExprModified >>(global_name,std::move(expr1)));

    //define表达式不产生可执行语句，往自己所处环境里加入自己的名字，并将自身提升至全局符号
    return ResultType(nullptr, func_env,
                      std::move(fundefs));
}


ResultType
check_arith(unique_ptr<Expr> expr, const shared_ptr<SymbolTable<FuncSignature>> &env, const string &name_modifier,
            const std::function<unique_ptr<Expr>(vector<unique_ptr<Expr>>)> &arith_type) {

    auto[t, arglist]=std::move(std::get<Expr::arith_expr>(*(*expr)));
    auto symbol_table = FuncDefs();

    for (auto &item:arglist) {
        auto[expr, a, symbols]=symantic_check(std::move(item), env, name_modifier);
        symbol_table = std::move(symbol_table) + std::move(symbols);
        item = std::move(expr);
    }

    return ResultType(arith_type(std::move(arglist)), env, std::move(symbol_table));
}

ResultType
check_id(unique_ptr<Expr> expr, const shared_ptr<SymbolTable<FuncSignature>> &env, ScopeModifier name_modifier) {
    try {
        //确认当前id是否为某函数的名字，如果是，此处引用应使用mangle后的名字
        auto &[id_name]=std::get<Expr::id_expr>(*(*expr));
        return ResultType(Expr::IdExpr(env->find(id_name)->global_name), env, FuncDefs());
    } catch (runtime_error &) {
        //如果没有找到该变量，则视其为全局符号，在调用时确认该符号是否在全局环境里
        return ResultType(std::move(expr), env, FuncDefs());
    }
}


ResultType
check_apply(unique_ptr<Expr> expr, const shared_ptr<SymbolTable<FuncSignature>> &env, ScopeModifier name_modifier) {

    //将参数列表移出来
    auto[arglist]=std::move(std::get<Expr::apply_expr>(*(*expr)));
    auto symbol_table = FuncDefs();
    auto count = 0;
    for (auto &item:arglist) {
        //通过为每个表达式开一个虚拟scope,防止不同操作子的define with 子句的符号被提升到相同全局名
        auto[expr, a, symbols]=symantic_check(std::move(item), env, name_modifier + '@' + to_string(count));
        symbol_table = std::move(symbol_table) + std::move(symbols);
        item = std::move(expr);
        count++;
    }

    return ResultType(Expr::ApplyExpr(std::move(arglist)), env, std::move(symbol_table));
}

ResultType
check_var_def(unique_ptr<Expr> expr, const shared_ptr<SymbolTable<FuncSignature>> &env, const string &name_modifier) {
    auto[var_name, value]=std::move(std::get<Expr::define_val>(*(*expr)));

    auto[expr_after_modify, new_env, tbl]=symantic_check(std::move(value), env, name_modifier);

    return ResultType(Expr::DefValExpr(var_name, std::move(expr_after_modify)), env, std::move(tbl));
}

ResultType
check_def_with_expr(unique_ptr<Expr> expr, const shared_ptr<SymbolTable<FuncSignature>> &env,
                    const string &name_modifier) {
    auto[def_part, expr1]=std::move(std::get<Expr::define_with_expr>(*(*expr)));

    //检查该定义语句，产生新的环境以及全局符号表,同时开启一个新的
    auto[obsolete1, new_env, new_symbols]=symantic_check(std::move(def_part), env, name_modifier + '@');

    auto[result_expr, ob_env, result_symbols]=symantic_check(std::move(expr1), new_env, name_modifier);

    //返回一个处理过不带函数定义的表达式，不给所处环境增加新符号，同时将定义与表达式体里的符号返回
    if (!obsolete1)
        return ResultType(std::move(result_expr), env, std::move(new_symbols) + std::move(result_symbols));
    else
        return ResultType(Expr::DefWithExpr(std::move(obsolete1), std::move(result_expr)), env,
                          std::move(new_symbols) + std::move(result_symbols));
}

ResultType
symantic_check(unique_ptr<Expr> expr, const shared_ptr<SymbolTable<FuncSignature>> &env, const string &name_modifier) {
    switch (expr->getTag()) {
        case Expr::cond:
            return check_cond(std::move(expr), env, name_modifier);
        case Expr::eq:
            return check_relation(std::move(expr), env, Expr::EqExpr, name_modifier);
        case Expr::le:
            return check_relation(std::move(expr), env, Expr::LeExpr, name_modifier);
        case Expr::ge:
            return check_relation(std::move(expr), env, Expr::GeExpr, name_modifier);
        case Expr::lt:
            return check_relation(std::move(expr), env, Expr::LtExpr, name_modifier);
        case Expr::gt:
            return check_relation(std::move(expr), env, Expr::GtExpr, name_modifier);
        case Expr::id:
            return check_id(std::move(expr), env, name_modifier);
        case Expr::boolean:
            return ResultType(std::move(expr), env, FuncDefs());
        case Expr::interger:
            return ResultType(std::move(expr), env, FuncDefs());
        case Expr::apply:
            return check_apply(std::move(expr), env, name_modifier);
        case Expr::def_func:
            return check_def(std::move(expr), env, name_modifier);
        case Expr::plus:
            return check_arith(std::move(expr), env, name_modifier, Expr::PlusExpr);
        case Expr::minus:
            return check_arith(std::move(expr), env, name_modifier, Expr::MinusExpr);
        case Expr::times:
            return check_arith(std::move(expr), env, name_modifier, Expr::MultExpr);
        case Expr::div:
            return check_arith(std::move(expr), env, name_modifier, Expr::DivExpr);
        case Expr::def_with_expr:
            return check_def_with_expr(std::move(expr), env, name_modifier);
        case Expr::def_variable:
            return check_var_def(std::move(expr), env, name_modifier);
    }
}
