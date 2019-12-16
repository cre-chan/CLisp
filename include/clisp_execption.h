//
// Created by cre-chan on 19-12-16.
//

#ifndef SEMANTICS_CLISP_EXECPTION_H
#define SEMANTICS_CLISP_EXECPTION_H

#include <syntax.h>

class ClispError:
        public exception{
public:
    explicit ClispError(istream&);
    virtual const string info() const noexcept =0;
};

//用于标识不合法的字符
class UnrecognizedCharacter :
        public ClispError {
    char invalid_char;
public:
    UnrecognizedCharacter(char,istream&);

    const string info() const noexcept override;
};

class InvalidToken:
        public ClispError{
    string tok_name;
public:
    InvalidToken(istream &istream,string  tok);

    const string info() const noexcept override;
};

class UnmatchingBracket:
        public  ClispError{
public:
    explicit UnmatchingBracket(istream &istream);

    const string info() const noexcept override;
};

class PrimaryIsNotCallable:
        public ClispError{
    unique_ptr<Expr> expr;
public:
    PrimaryIsNotCallable(istream &istream, unique_ptr<Expr> expr);

    const string info() const noexcept override;
};

class ExpectingExpr:
        public ClispError{
public:
    explicit ExpectingExpr(istream &istream);

    const string info() const noexcept override;
};

class EmptyExpr:
        public  ClispError{
public:
    explicit EmptyExpr(istream &istream);

    const string info() const noexcept override;
};

class ExpectingOperands:
        public  ClispError{
    uint arg_num;
public:
    ExpectingOperands(istream &istream, uint argNum);

    const string info() const noexcept override;
};

class ExpectingIdOrArglist:
        public ClispError{
public:
    ExpectingIdOrArglist(istream &istream);

    const string info() const noexcept override;
};

#endif //SEMANTICS_CLISP_EXECPTION_H
