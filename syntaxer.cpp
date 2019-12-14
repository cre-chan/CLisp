//
// Created by cre-chan on 19-11-18.
//
#include "stdafx.h"
#include "syntax.h"

class Syntaxer{
    vector<variant<unique_ptr<Expr>,Token>>    operands;//记录中间结果
    vector<uint>                left_brackets;//记录遇到的左括号位置

    unique_ptr<Expr> reduce(uint);

    //归约不同形式算术表达式
    unique_ptr<Expr> reduce_arith(
            vector<variant<unique_ptr<Expr>,Token>>,
            const std::function<unique_ptr<Expr>(vector<unique_ptr<Expr>>)>&
    );

    unique_ptr<Expr> reduce_binary(
            vector<variant<unique_ptr<Expr>,Token>>,
            const std::function<unique_ptr<Expr>(unique_ptr<Expr>,unique_ptr<Expr>)>&
    );

public:
    Syntaxer();
    unique_ptr<Expr> parseExpr();

};

//构建一个默认的Syntaxer
Syntaxer::Syntaxer() : operands(), left_brackets() {}


//算数表达式的归约过程
unique_ptr<Expr> Syntaxer::reduce_arith(
        vector<variant<unique_ptr<Expr>, Token>> tempStack,
        const std::function<unique_ptr<Expr>(vector<unique_ptr<Expr>>)> &retType) {
    tempStack.pop_back();
    vector<unique_ptr<Expr>> ops;

    while (!tempStack.empty()) {
        ops.push_back(std::move(std::get<unique_ptr<Expr>>(tempStack.back())));
        tempStack.pop_back();
    }

    return retType(std::move(ops));
}


unique_ptr<Expr> Syntaxer::reduce_binary(vector<variant<unique_ptr<Expr>, Token>> tempStack,
                                         const std::function<unique_ptr<Expr>(unique_ptr<Expr>,
                                                                              unique_ptr<Expr>)> &retType) {
    tempStack.pop_back();
    auto op1 = std::get<unique_ptr<Expr>>(std::move(tempStack.back()));
    tempStack.pop_back();
    auto op2 = std::get<unique_ptr<Expr>>(std::move(tempStack.back()));
    tempStack.pop_back();

    return retType(std::move(op1), std::move(op2));
}


//移出一个括号并继续
unique_ptr<Expr> Syntaxer::reduce(uint upper_bound) {
    vector<variant<unique_ptr<Expr>, Token>> tempStack;
    //todo:处理括号不对称的情况

    for (auto element = std::move(operands.back());
         element.index() != 1 || std::get<Token>(element).getTag() != Token::lbr; element = std::move(
            operands.back())) {
        tempStack.push_back(std::move(element));
        operands.pop_back();
    }

    operands.pop_back();

    //将列表中第一位取出
    variant<unique_ptr<Expr>, Token> &oper = tempStack.back();

    if (oper.index() == 1) {
        //若该操作数是一个Token
        switch (std::get<Token>(oper).getTag()) {

            //排除掉会直接转化为expr的token
            case Token::def: {
                tempStack.pop_back();
                auto arglist = std::move(std::get<unique_ptr<Expr>>(tempStack.back()));
                tempStack.pop_back();

                if (arglist->getTag() == Expr::expr_type::id) {
                    auto expr_body = std::move(std::get<unique_ptr<Expr>>(tempStack.back()));
                    auto variable_name = std::move(std::get<Expr::id_expr>(*(*arglist)));
                    tempStack.pop_back();
                    return Expr::DefValExpr(variable_name.name, std::move(expr_body));
                } else if (arglist->getTag() == Expr::expr_type::apply) {
                    vector<unique_ptr<Expr>> arguments = std::move(std::get<Expr::apply_expr>(*(*arglist)).list);
                    vector<Expr::id_expr> arg_names;
                    auto expr_body = std::move(std::get<unique_ptr<Expr>>(tempStack.back()));
                    tempStack.pop_back();
                    for (auto &id:arguments) {
                        //将ID表达式移出
                        Expr::id_expr variable_name = std::move(std::get<Expr::id_expr>(*(*id)));
                        arg_names.push_back(variable_name);
                    }
                    arguments.clear();
                    return Expr::DefExpr(std::move(arg_names), std::move(expr_body));
                } else
                    throw exception();
            }
            case Token::cond: {
                tempStack.pop_back();
                auto cond = std::move(std::get<unique_ptr<Expr>>(tempStack.back()));
                tempStack.pop_back();
                auto expr1 = std::move(std::get<unique_ptr<Expr>>(tempStack.back()));
                tempStack.pop_back();
                auto expr2 = std::move(std::get<unique_ptr<Expr>>(tempStack.back()));
                tempStack.pop_back();
                return Expr::IfExpr(std::move(cond), std::move(expr1), std::move(expr2));
            }
            case Token::plus:
                return reduce_arith(std::move(tempStack), Expr::PlusExpr);
            case Token::minus:
                return reduce_arith(std::move(tempStack), Expr::MinusExpr);
            case Token::mul:
                return reduce_arith(std::move(tempStack), Expr::MultExpr);
            case Token::div:
                return reduce_arith(std::move(tempStack), Expr::DivExpr);
            case Token::eq:
                return reduce_binary(std::move(tempStack), Expr::EqExpr);
            case Token::le:
                return reduce_binary(std::move(tempStack), Expr::LeExpr);
            case Token::ge:
                return reduce_binary(std::move(tempStack), Expr::GeExpr);
            case Token::lt:
                return reduce_binary(std::move(tempStack), Expr::LtExpr);
            case Token::gt:
                return reduce_binary(std::move(tempStack), Expr::GtExpr);
            case Token::and_op:
                return reduce_binary(std::move(tempStack), Expr::AndExpr);
            case Token::or_op:
                return reduce_binary(std::move(tempStack), Expr::OrExpr);
            case Token::not_op: {
                tempStack.pop_back();
                auto expr = std::move(std::get<unique_ptr<Expr>>(tempStack.back()));
                tempStack.pop_back();
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
            case Expr::apply: {
                vector<unique_ptr<Expr>> arglist;
                arglist.push_back(std::move(std::get<unique_ptr<Expr>>(oper)));
                tempStack.pop_back();
                while (!tempStack.empty()) {
                    arglist.push_back(std::move(std::get<unique_ptr<Expr>>(tempStack.back())));
                    tempStack.pop_back();
                }
                return Expr::ApplyExpr(std::move(arglist));
            }
            case Expr::def_variable:
            case Expr::def_func: {
                auto def = std::move(std::get<unique_ptr<Expr>>(oper));
                tempStack.pop_back();
                vector<unique_ptr<Expr>> exprs;

                exprs.push_back(std::move(def));

                while (std::get<unique_ptr<Expr>>(tempStack.back())->getTag() == Expr::def_func ||
                       std::get<unique_ptr<Expr>>(tempStack.back())->getTag() == Expr::def_variable) {
                    exprs.push_back(std::move(std::get<unique_ptr<Expr>>(tempStack.back())));
                    tempStack.pop_back();
                }

                auto expr_body = std::move(std::get<unique_ptr<Expr>>(tempStack.back()));
                tempStack.pop_back();
                auto def_with = Expr::DefWithExpr(std::move(exprs.back()), std::move(expr_body));
                exprs.pop_back();
                while (!exprs.empty()) {
                    def_with = Expr::DefWithExpr(std::move(exprs.back()), std::move(def_with));
                    exprs.pop_back();
                }
                return std::move(def_with);
            }

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
                //todo:错误处理
                throw exception();//出现语法错误
        }

    }

}

unique_ptr<Expr> Syntaxer::parseExpr() {
    Token token = Token::Default();

    do {
        cin >> token;
        switch (token.getTag()) {
            case Token::lbr:
                //记录当前栈的大小
                operands.emplace_back(token);
                break;
            case Token::rbr: {
                //遇到右括号则归约,并将归约结果放在最后
                operands.emplace_back(reduce(operands.size()));
                break;
            }
                //对于标点符号，原样保留符号
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
                throw exception();
                break;
        }
    } while (operands.front().index()==1);

    //从栈中移出最终结果，并清空栈
    auto ret = std::move(std::get<unique_ptr<Expr>>(operands.back()));
    operands.pop_back();


    return std::move(ret);
}

istream &operator>>(istream &in, unique_ptr<Expr> &a) {
    a=Syntaxer().parseExpr();
    return in;
}