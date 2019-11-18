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
    struct if_caluse {
        unique_ptr<Expr> condition, expr1, expr2;
    };

    // (not expr)
    struct not_expr {
        unique_ptr<Expr> expr;
    };

    // (and expr1 expr2)
    struct and_expr {
        unique_ptr<Expr> expr1, expr2;
    };

    // (or expr1 expr2)
    struct or_expr {
        unique_ptr<Expr> expr1, expr2;
    };

    // id
    struct id_expr {
        string name;
    };

    // true|false
    struct bool_expr {
        bool val;
    };

    // interger
    struct int_expr {
        int val;
    };

    // (expr1 expr2 ... exprn)
    struct apply_expr {
        vector<unique_ptr<Expr>> list;
    };

    // (define (func x1...xn) expr)|(define id expr)
    struct define_expr {
        vector<id_expr>  arglist;
        unique_ptr<Expr> body;
    };

    // (+|-|*|/ expr1 expr2 ... exprn)
    struct arith_expr {
        enum op_type {
            plus,
            minus,
            times,
            div
        } tag;

        vector<unique_ptr<Expr>> operlist;
    };

    // (define expr)
    struct define_with_expr{
        define_expr def_part;
        unique_ptr<Expr> expr;
    };

    // (= < > <= >= expr1 expr2)
    struct binary_relation{
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
        neg,    //否定
        conj,   //合取
        disj,   //析取
        eq,
        le,
        ge,
        lt,
        gt,
        id,     //标识符
        boolean,    //布尔
        interger,   //整数
        apply,      //函数
        def,        //定义
        plus,
        minus,
        times,
        div,         //算术
        def_with_expr   //有define部分的表达式
    };

    //将op_type的定义提到Expr里
    using op_type=arith_expr::op_type;
    using variant_t=variant<
            if_caluse, not_expr, and_expr, or_expr, id_expr,
            bool_expr, int_expr, apply_expr, define_expr, arith_expr,define_with_expr,binary_relation>;
    using rel_type=binary_relation::rel_type ;
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

    static unique_ptr<Expr> DefExpr(vector<id_expr>, unique_ptr<Expr>);

    static unique_ptr<Expr> PlusExpr(vector<unique_ptr<Expr>>);

    static unique_ptr<Expr> MinusExpr(vector<unique_ptr<Expr>>);

    static unique_ptr<Expr> MultExpr(vector<unique_ptr<Expr>>);

    static unique_ptr<Expr> DivExpr(vector<unique_ptr<Expr>>);

    static unique_ptr<Expr> DefWithExpr(define_expr,unique_ptr<Expr>);

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
    Expr(expr_type, vector<unique_ptr<Expr>>);  //函数调用
    Expr(expr_type, unique_ptr<Expr>);  //NOT表达式
    Expr(expr_type, unique_ptr<Expr>, unique_ptr<Expr>);    //'AND表达式OR表达式,以及关系表达式
    Expr(expr_type, vector<id_expr>, unique_ptr<Expr>);     // DEFINE表达式
    Expr(expr_type, define_expr,unique_ptr<Expr>);
    Expr(expr_type, unique_ptr<Expr>, unique_ptr<Expr>, unique_ptr<Expr>); //if表达式
};




class Syntaxer{
    vector<variant<unique_ptr<Expr>,Token>>    operands;//记录中间结果
    vector<uint>                left_brackets;//记录遇到的左括号位置
    Lexer                       input_stream;

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
#endif //LEXER_SYNTAX_H
