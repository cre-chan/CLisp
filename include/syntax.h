//
// Created by cre-chan on 19-11-16.
//

#ifndef LEXER_SYNTAX_H
#define LEXER_SYNTAX_H

#include <lexer.h>

using namespace std;

/*
 * 本文件定义了CLisp的抽象语法树，并使用一个基于栈的S-R解析器进行解析
 * */

class Expr {
public:
    // (if cond epr1 expr2)
    struct if_caluse_t {
        unique_ptr<Expr> condition, expr1, expr2;
    };

    // id
    struct id_expr_t {
        string name;
    };

    // true|false
    struct bool_expr_t {
        bool val;
    };

    // interger
    struct int_expr_t {
        int val;
    };

    // (expr1 expr2 ... exprn)
    struct apply_expr_t {
        vector<unique_ptr<Expr>> list;
    };

    // (define (func x1...xn) expr)|(define id expr)
    struct define_func_t {
        vector<id_expr_t>  arglist;
        unique_ptr<Expr> body;
    };

    struct define_val_t{
        string name;
        unique_ptr<Expr> body;
    };

    // (+|-|*|/ expr1 expr2 ... exprn)
    struct arith_expr_t {
        enum op_type {
            plus,
            minus,
            times,
            div
        } tag;

        vector<unique_ptr<Expr>> operlist;
    };

    // (define expr)
    struct define_with_expr_t{
        unique_ptr<Expr> def_part;
        unique_ptr<Expr> expr;
    };

    // (= < > <= >= expr1 expr2)
    struct binary_relation_t{
        enum rel_type{
            eq,
            le,
            ge,
            lt,
            gt,
        }tag;
        unique_ptr<Expr> expr1;
        unique_ptr<Expr> expr2;
    };

    enum expr_type {
        cond,   //条件
        eq,
        le,
        ge,
        lt,
        gt,
        id,     //标识符
        boolean,    //布尔
        interger,   //整数
        apply,      //函数
        def_func,   //定义
        plus,
        minus,
        times,
        div,         //算术
        def_with_expr,   //有define部分的表达式
        def_variable
    };

    //将op_type的定义提到Expr里
    using op_type=arith_expr_t::op_type;
    using variant_t=variant<
            if_caluse_t, id_expr_t,
            bool_expr_t, int_expr_t, apply_expr_t, define_func_t, arith_expr_t,define_with_expr_t,binary_relation_t,define_val_t>;
    using rel_type=binary_relation_t::rel_type ;
    //获得该节点的类型
    expr_type getTag() const;

    ostream& prettyPrint(ostream&,uint) const;

    //类型安全地创建各表达式
    static unique_ptr<Expr> IfExpr(unique_ptr<Expr>, unique_ptr<Expr>, unique_ptr<Expr>);

    static unique_ptr<Expr> NegExpr(unique_ptr<Expr>);

    static unique_ptr<Expr> AndExpr(unique_ptr<Expr>, unique_ptr<Expr>);

    static unique_ptr<Expr> OrExpr(unique_ptr<Expr>, unique_ptr<Expr>);

    static unique_ptr<Expr> EqExpr(unique_ptr<Expr>, unique_ptr<Expr>);

    static unique_ptr<Expr> LeExpr(unique_ptr<Expr>, unique_ptr<Expr>);

    static unique_ptr<Expr> LtExpr(unique_ptr<Expr>, unique_ptr<Expr>);

    static unique_ptr<Expr> GeExpr(unique_ptr<Expr>, unique_ptr<Expr>);

    static unique_ptr<Expr> GtExpr(unique_ptr<Expr>, unique_ptr<Expr>);

    static unique_ptr<Expr> IdExpr(const string&);

    static unique_ptr<Expr> BooleanExpr(bool);

    static unique_ptr<Expr> IntExpr(int);

    static unique_ptr<Expr> ApplyExpr(vector<unique_ptr<Expr>>);

    static unique_ptr<Expr> DefExpr(vector<id_expr_t>, unique_ptr<Expr>);

    static unique_ptr<Expr> PlusExpr(vector<unique_ptr<Expr>>);

    static unique_ptr<Expr> MinusExpr(vector<unique_ptr<Expr>>);

    static unique_ptr<Expr> MultExpr(vector<unique_ptr<Expr>>);

    static unique_ptr<Expr> DivExpr(vector<unique_ptr<Expr>>);

    static unique_ptr<Expr> DefWithExpr(unique_ptr<Expr>,unique_ptr<Expr>);

    static unique_ptr<Expr> DefValExpr(const string&,unique_ptr<Expr> body);

    //以带缩进格式输出抽象语法树
    friend ostream &operator<<(ostream &, const Expr &);

    //重载解引用运算符, 方便解构
    variant_t& operator*();

private:

    variant_t content;

    ostream& indentation(ostream&,uint) const;
    ostream& relaional_output(ostream &, uint, const string&) const;
    template <class primary_T>
    ostream& primary_output(ostream&,uint,const string&,std::function<string(const primary_T&)>) const;

    Expr(expr_type, bool);          //布尔型字面量
    Expr(expr_type, int);           //整形字面量
    Expr(expr_type, const string &); //ID表达式
    Expr(expr_type, const string &,unique_ptr<Expr>); //ID表达式
    Expr(expr_type, vector<unique_ptr<Expr>>);  //函数调用
    Expr(expr_type, unique_ptr<Expr>, unique_ptr<Expr>);    //关系表达式
    Expr(expr_type, vector<id_expr_t>, unique_ptr<Expr>);     // DEFINE表达式
    Expr(expr_type, unique_ptr<Expr>, unique_ptr<Expr>, unique_ptr<Expr>); //if表达式
};


istream& operator>>(istream&,unique_ptr<Expr>&);



#endif //LEXER_SYNTAX_H
