//
// Created by cre-chan on 19-11-16.
//

#ifndef LEXER_SYNTAX_H
#define LEXER_SYNTAX_H

#include <lexer.h>

using namespace std;

class Expr {
public:
    struct if_caluse {
        unique_ptr<Expr> condition, expr1, expr2;
    };

    struct not_expr {
        unique_ptr<Expr> expr;
    };

    struct and_expr {
        unique_ptr<Expr> expr1, expr2;
    };

    struct or_expr {
        unique_ptr<Expr> expr1, expr2;
    };

    struct id_expr {
        string name;
    };

    struct bool_expr {
        bool val;
    };

    struct int_expr {
        int val;
    };

    struct apply_expr {
        vector<unique_ptr<Expr>> list;
    };

    struct define_expr {
        vector<id_expr>  arglist;
        unique_ptr<Expr> body;
    };

    struct arith_expr {
        enum op_type {
            plus,
            minus,
            times,
            div
        } tag;

        vector<unique_ptr<Expr>> operlist;
    };

    struct define_with_expr{
        define_expr def_part;
        unique_ptr<Expr> expr;
    };

    enum expr_type {
        cond,   //条件
        neg,    //否定
        conj,   //合取
        disj,   //析取
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

    using op_type=arith_expr::op_type;
    using variant_t=variant<
            if_caluse, not_expr, and_expr, or_expr, id_expr,
            bool_expr, int_expr, apply_expr, define_expr, arith_expr,define_with_expr>;
    //获得该节点的类型
    expr_type getTag() const;

    ostream& prettyPrint(ostream&,uint) const;

    //类型安全地创建各表达式
    static unique_ptr<Expr> IfExpr(unique_ptr<Expr>, unique_ptr<Expr>, unique_ptr<Expr>);

    static unique_ptr<Expr> NegExpr(unique_ptr<Expr>);

    static unique_ptr<Expr> AndExpr(unique_ptr<Expr>, unique_ptr<Expr>);

    static unique_ptr<Expr> OrExpr(unique_ptr<Expr>, unique_ptr<Expr>);

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

    friend ostream &operator<<(ostream &, const Expr &);

private:

    variant_t content;

    ostream& indentation(ostream&,uint) const;


    Expr(expr_type, bool);          //布尔型字面量
    Expr(expr_type, int);           //整形字面量
    Expr(expr_type, const string &); //ID表达式
    Expr(expr_type, vector<unique_ptr<Expr>>);  //函数调用
    Expr(expr_type, unique_ptr<Expr>);  //NOT表达式
    Expr(expr_type, unique_ptr<Expr>, unique_ptr<Expr>);    //'AND表达式OR表达式
    Expr(expr_type, vector<id_expr>, unique_ptr<Expr>);     // DEFINE表达式
    Expr(expr_type, define_expr,unique_ptr<Expr>);
    Expr(expr_type, unique_ptr<Expr>, unique_ptr<Expr>, unique_ptr<Expr>); //if表达式
};

#endif //LEXER_SYNTAX_H
