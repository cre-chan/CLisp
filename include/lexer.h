//
// Created by cre-chan on 19-11-15.
//

#ifndef CLISP_LEXER_H
#define CLISP_LEXER_H

//#include <stdafx.h>

using namespace std;

/* Token代表一个输入流中出现的符号
 * */
class Token{
public:
    enum TokenType{
        lbr,
        rbr,
        def,
        cond,   //if instead of cond
        identifier,
        interger,
        plus,
        minus,
        mul,
        div,
        nextline,
        boolean,
        eq,
        le,
        ge,
        lt,
        gt,
        and_op,
        or_op,
        not_op,
        eof,
        comment
    };

    TokenType   getTag() const;
    string      getIdentifierValue() const;
    int         getIntValue() const;
    bool        getBooleanValue() const;
    friend ostream& operator <<(ostream&,const Token&);

    /*
     * 使用这些函数来构建Token实例，以规避tag与数据域类型不一致的错误
     * */
    static Token    Lbr();          //左括号
    static Token    Rbr();          //右括号
    static Token    Def();          //define关键字
    static Token    Cond();         //if关键字
    static Token    Identifier(const string&); //标识符
    static Token    Interger(int);  //整数字面量
    static Token    Plus();         //+运算符
    static Token    Minus();        //-运算符
    static Token    Mul();          //*运算符
    static Token    Div();          //运算符
    static Token    Eof();          //文件的结束
    static Token    Boolean(bool);  //布尔型字面量
    static Token    Eq();           //=运算符
    static Token    Le();           //<=运算符
    static Token    Ge();           //>=运算符
    static Token    Lt();           //<运算符
    static Token    Gt();           //>运算符
    static Token    And();          //and运算符
    static Token    Or();           //or运算符
    static Token    Not();          //not 运算符
    static Token    Default();
    static Token    Comment();
private:
    TokenType   tag;
    variant<monostate,int,string,bool> content;

    Token();
    explicit Token(TokenType);
    Token(TokenType, const string&);
    Token(TokenType, int);
    Token(TokenType,bool);
};


istream& operator>>(istream&,Token&);


#endif //CLISP_LEXER_H
