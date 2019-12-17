//
// Created by cre-chan on 19-11-18.
//
#include "stdafx.h"
#include <clisp_execption.h>
#include "syntax.h"


//从栈中取出项，并保证该项
unique_ptr<Expr> yield_expr_from(vector<variant<unique_ptr<Expr>, Token>> &tempStack, istream &in) {
    try {
        auto ret = std::move(std::get<unique_ptr<Expr>>(tempStack.back()));
        tempStack.pop_back();
        return std::move(ret);
    } catch (bad_variant_access &) {
        throw ExpectingExpr(in);
    }
}

//算数表达式的归约过程
unique_ptr<Expr> reduce_arith(
        vector<variant<unique_ptr<Expr>, Token>> tempStack,
        const std::function<unique_ptr<Expr>(vector<unique_ptr<Expr>>)> &retType, istream &in) {
    vector<unique_ptr<Expr>> ops;

    while (!tempStack.empty())
        ops.push_back(yield_expr_from(tempStack, in));

    //如果操作数小于2则报错
    if (ops.size() < 2)
        throw ExpectingOperands(in, ops.size());

    return retType(std::move(ops));
}


unique_ptr<Expr> reduce_binary(vector<variant<unique_ptr<Expr>, Token>> tempStack,
                               const std::function<unique_ptr<Expr>(unique_ptr<Expr>,
                                                                    unique_ptr<Expr>)> &retType, istream &in) {
    //如果操作数数量不是2则报错
    if (tempStack.size() != 2)
        throw ExpectingOperands(in, tempStack.size());

    //依赖于求值顺序
    return retType(yield_expr_from(tempStack, in), yield_expr_from(tempStack, in));
}

unique_ptr<Expr>
reduce_def_with(vector<variant<unique_ptr<Expr>, Token>> &tempStack, variant<unique_ptr<Expr>, Token> &oper,
                istream &in) {
    auto def = move(get<unique_ptr<Expr>>(oper));

    vector<unique_ptr<Expr>> exprs;

    exprs.push_back(move(def));

    auto expr = yield_expr_from(tempStack, in);
    for (;
            expr->getTag() == Expr::def_func || expr->getTag() == Expr::def_variable;
            expr = yield_expr_from(tempStack, in))
        exprs.push_back(std::move(expr));

    tempStack.emplace_back(std::move(expr));

    auto expr_body = yield_expr_from(tempStack, in);

    auto def_with = Expr::DefWithExpr(move(exprs.back()), move(expr_body));
    exprs.pop_back();
    while (!exprs.empty()) {
        def_with = Expr::DefWithExpr(move(exprs.back()), move(def_with));
        exprs.pop_back();
    }
    return move(def_with);
}

unique_ptr<Expr> reduce_def(vector<variant<unique_ptr<Expr>, Token>> &tempStack, istream &in) {

    if (tempStack.size() != 2)
        throw ExpectingOperands(in, 2);

    auto arglist = yield_expr_from(tempStack, in);

    if (arglist->getTag() == Expr::id) {
        auto expr_body = yield_expr_from(tempStack, in);
        auto variable_name = move(get<Expr::id_expr>(*(*arglist)));
        return Expr::DefValExpr(variable_name.name, move(expr_body));
    } else if (arglist->getTag() == Expr::apply) {
        vector<unique_ptr<Expr>> arguments = move(get<Expr::apply_expr>(*(*arglist)).list);
        vector<Expr::id_expr> arg_names;
        auto expr_body = yield_expr_from(tempStack, in);
        for (auto &id:arguments) {
            //将ID表达式移出
            Expr::id_expr variable_name = move(get<Expr::id_expr>(*(*id)));
            arg_names.push_back(variable_name);
        }
        arguments.clear();
        return Expr::DefExpr(move(arg_names), move(expr_body));
    } else
        throw ExpectingIdOrArglist(in);
}

unique_ptr<Expr>
reduce_apply(vector<variant<unique_ptr<Expr>, Token>> &tempStack, variant<unique_ptr<Expr>, Token> &oper, istream &in) {
    vector<unique_ptr<Expr>> arglist;
    arglist.push_back(move(get<unique_ptr<Expr>>(oper)));

    while (!tempStack.empty())
        arglist.push_back(yield_expr_from(tempStack, in));

    return Expr::ApplyExpr(move(arglist));
}

unique_ptr<Expr> reduce_if(vector<variant<unique_ptr<Expr>, Token>> &tempStack, istream &in) {
    if (tempStack.size() != 3)
        throw ExpectingOperands(in, 3);

    return Expr::IfExpr(yield_expr_from(tempStack, in),
                        yield_expr_from(tempStack, in),
                        yield_expr_from(tempStack, in));
}

//移出一个括号并继续
unique_ptr<Expr> reduce(vector<variant<unique_ptr<Expr>, Token>> &operands, istream &in) {
    vector<variant<unique_ptr<Expr>, Token>> tempStack;


    auto fetch_term = [&in, &operands]() {
        if (operands.empty())
            throw UnmatchingBracket(in);

        auto element = std::move(operands.back());
        operands.pop_back();

        return element;
    };

    //从operands中取出各项元素
    for (auto element = fetch_term();
         element.index() != 1 || std::get<Token>(element).getTag() != Token::lbr; element = fetch_term())
        tempStack.push_back(std::move(element));

    //如果没有操作数则报错
    if (tempStack.empty())
        throw EmptyExpr(in);

    //将列表中第一位取出
    variant<unique_ptr<Expr>, Token> oper = std::move(tempStack.back());
    tempStack.pop_back();

    if (oper.index() == 1) {
        //若该操作数是一个Token
        switch (std::get<Token>(oper).getTag()) {
            //排除掉会直接转化为expr的token
            case Token::def:
                return reduce_def(tempStack, in);
            case Token::cond:
                return reduce_if(tempStack, in);
            case Token::plus:
                return reduce_arith(std::move(tempStack), Expr::PlusExpr, in);
            case Token::minus:
                return reduce_arith(std::move(tempStack), Expr::MinusExpr, in);
            case Token::mul:
                return reduce_arith(std::move(tempStack), Expr::MultExpr, in);
            case Token::div:
                return reduce_arith(std::move(tempStack), Expr::DivExpr, in);
            case Token::eq:
                return reduce_binary(std::move(tempStack), Expr::EqExpr, in);
            case Token::le:
                return reduce_binary(std::move(tempStack), Expr::LeExpr, in);
            case Token::ge:
                return reduce_binary(std::move(tempStack), Expr::GeExpr, in);
            case Token::lt:
                return reduce_binary(std::move(tempStack), Expr::LtExpr, in);
            case Token::gt:
                return reduce_binary(std::move(tempStack), Expr::GtExpr, in);
            case Token::and_op:
                return reduce_binary(std::move(tempStack), Expr::AndExpr, in);
            case Token::or_op:
                return reduce_binary(std::move(tempStack), Expr::OrExpr, in);
            case Token::not_op: {
                auto expr = yield_expr_from(tempStack, in);
                return Expr::NegExpr(std::move(expr));
            }
            default:
                throw exception();
        }
    } else {
        switch (std::get<unique_ptr<Expr>>(oper)->getTag()) {
            case Expr::cond:
            case Expr::id:
            case Expr::def_with_expr:
            case Expr::apply:
                return reduce_apply(tempStack, oper, in);
            case Expr::def_variable:
            case Expr::def_func:
                return reduce_def_with(tempStack, oper, in);
                //基础类型不是applicative的
            case Expr::eq:
            case Expr::le:
            case Expr::ge:
            case Expr::lt:
            case Expr::gt:
            case Expr::boolean:
            case Expr::interger:
            case Expr::plus:
            case Expr::minus:
            case Expr::times:
            case Expr::div:
            default:
                throw PrimaryIsNotCallable(in, std::move(std::get<unique_ptr<Expr>>(oper)));//出现语法错误
        }
    }

}


istream &operator>>(istream &in, unique_ptr<Expr> &a) {
    Token token = Token::Default();
    vector<variant<unique_ptr<Expr>, Token>> operands;
    do {
        in >> token;
        switch (token.getTag()) {
            case Token::rbr:
                //遇到右括号则归约,并将归约结果放在最后
                operands.emplace_back(reduce(operands, in));
                break;
                //对于标点符号，原样保留符号
            case Token::lbr:
            case Token::eq:
            case Token::le:
            case Token::ge:
            case Token::lt:
            case Token::gt:
            case Token::and_op:
            case Token::or_op:
            case Token::not_op:
            case Token::cond:
            case Token::plus:
            case Token::minus:
            case Token::mul:
            case Token::div:
            case Token::def:
                operands.emplace_back(token);
                break;
            case Token::identifier:
                //当遇到标识符时，直接生成标识符
                operands.emplace_back(Expr::IdExpr(token.getIdentifierValue()));
                break;
            case Token::interger:
                operands.emplace_back(Expr::IntExpr(token.getIntValue()));
                break;
            case Token::boolean:
                operands.emplace_back(Expr::BooleanExpr(token.getBooleanValue()));
                break;
            case Token::nextline:
                break;
            case Token::eof:
                //跳出
                if (operands.empty())
                    throw NormalExit();
                else
                    throw UnexpectedEOF(in);
        }
    } while (operands.front().index() == 1);

    //从栈中移出最终结果，并清空栈
    a = yield_expr_from(operands, in);


    return in;
}