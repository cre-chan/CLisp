#include <stdafx.h>
#include <lexer.h>

//
// Created by cre-chan on 19-11-15.
//




using TokenType=Token::TokenType;


//创建一个只有tag的Token
Token::Token(TokenType t) : tag(t) {}

//创建一个Int
Token::Token(TokenType t, int i) : tag(t), content(i) {}

//创建一个标识符
Token::Token(TokenType t, const string& s) : tag(t), content(s) {}

Token::Token(Token::TokenType t, bool literal) : tag(t), content(literal) {}

TokenType Token::getTag() const { return tag; }

string Token::getIdentifierValue() const {
    return get<string>(content);
}

int Token::getIntValue() const {
    return get<int>(content);
}

bool Token::getBooleanValue() const {
    return get<bool>(content);
}


/*
 * 特殊的工厂函数，用于创建类型安全的API
 * */
Token Token::Lbr() { return Token(TokenType::lbr); }

Token Token::Rbr() {return Token(TokenType::rbr);}

Token Token::Def() { return Token(TokenType::def); }

Token Token::Cond() { return Token(TokenType::cond); }

Token Token::Identifier(const string& name) { return Token(TokenType::identifier, name); }

Token Token::Interger(int i) { return Token(TokenType::interger, i); }

Token Token::Minus() { return Token(TokenType::minus); }

Token Token::Plus() { return Token(TokenType::plus); }

Token Token::Mul() { return Token(TokenType::mul); }

Token Token::Div() { return Token(TokenType::div); }

Token Token::Eof() { return Token(TokenType::eof); }

Token Token::Nextline() { return Token(TokenType::nextline); }

Token Token::Not() { return Token(TokenType::not_op); }

Token Token::And() { return Token(TokenType::and_op); }

Token Token::Or() { return Token(TokenType::or_op); }

Token Token::Eq() { return Token(TokenType::eq); }

Token Token::Lt() { return Token(TokenType::lt); }

Token Token::Gt() { return Token(TokenType::gt); }

Token Token::Ge() {return Token(TokenType::ge);}

Token Token::Le() {return Token(TokenType::le);}

Token Token::Boolean(bool b) {return Token(TokenType::boolean,b);}

// Token's output
ostream &operator<<(ostream &out, const Token &self) {
    switch (self.getTag()) {
        case TokenType::mul:
            out << "mult";
            break;
        case TokenType::plus:
            out << "plus";
            break;
        case TokenType::minus:
            out << "minus";
            break;
        case TokenType::interger:
            out << "int(" << self.getIntValue() << ")";
            break;
        case TokenType::identifier:
            out << "id(" << self.getIdentifierValue() << ")";
            break;
        case TokenType::eof:
            out << "eof";
            break;
        case TokenType::div:
            out << "div";
            break;
        case TokenType::cond:
            out << "if";
            break;
        case TokenType::def:
            out << "def";
            break;
        case TokenType::lbr:
            out << "LBR";
            break;
        case TokenType::rbr:
            out<<"RBR";
            break;
        case TokenType::nextline:
            out << endl;
            break;
        case TokenType::boolean:
            out << "boolean("<<(self.getBooleanValue()?"true":"false")<<")";
            break;
        case TokenType::ge:
            out << "ge";
            break;
        case TokenType::or_op:
            out<<"or";
            break;
        case TokenType::and_op:
            out<<"and";
            break;
        case TokenType::not_op:
            out<<"not";
            break;
        case TokenType::gt:
            out<<"gt";
            break;
        case TokenType::le:
            out<<"le";
            break;
        case TokenType::lt:
            out<<"lt";
            break;
        case TokenType::eq:
            out<<"eq";
            break;
    }

    return out;
}


/*
 * Lexer暂存一个上下文，用于读入
 *
 * */
Lexer::Lexer() :
    temp_c('\t'), is_c_out_of_date(true),ln(1),offset(0) {}

char Lexer::skipAndReturn() {
    offset++;
    return cin.get();
}

//msd为most significant digit的缩写，即指定可选的最高位
int Lexer::parseInt() {
    int sum=0;
    while (isdigit(temp_c)) {
        sum = sum * 10 + temp_c - '0';
        temp_c = skipAndReturn();
    }
    return sum;
}

Token Lexer::getToken() {
    //避免重复读入
    if (is_c_out_of_date) {
        temp_c = skipAndReturn();
        is_c_out_of_date = false;
    }

    //跳过所有空格和tab
    while (
            isblank(temp_c)
            )
        temp_c = skipAndReturn();

    //simple token
    switch (temp_c) {
        case '(':
            is_c_out_of_date = true;
            return Token::Lbr();
        case ')':
            is_c_out_of_date = true;
            return Token::Rbr();
        case '*':
            is_c_out_of_date = true;
            return Token::Mul();
        case '-':
            temp_c=skipAndReturn();
            if (isdigit(temp_c))
                return Token::Interger(-parseInt());
            return Token::Minus();
        case '+':
            is_c_out_of_date = true;
            return Token::Plus();
        case '/':
            is_c_out_of_date = true;
            return Token::Div();
        case '\n':
            is_c_out_of_date = true;
            offset=0;
            ln++;
            return Token::Nextline();
        case '=':
            is_c_out_of_date = true;
            return Token::Eq();
        case '<':
            if ((temp_c = skipAndReturn()) == '=') {
                is_c_out_of_date = true;
                return Token::Le();
            }

            return Token::Lt();
        case '>':
            if ((temp_c = skipAndReturn()) == '=') {
                is_c_out_of_date = true;
                return Token::Ge();
            }

            return Token::Gt();
        case EOF:
            return Token::Eof();

    }

    //some identifier
    if (isalpha(temp_c)||temp_c=='_') {
        string buf;

        while (isalnum(temp_c)||temp_c=='_') {
            buf.push_back(temp_c);
            temp_c = skipAndReturn();
        }

        if (buf == "define")
            return Token::Def();
        else if (buf == "if")
            return Token::Cond();
        else if (buf == "true")
            return Token::Boolean(true);
        else if (buf == "false")
            return Token::Boolean(false);
        else if (buf == "and")
            return Token::And();
        else if (buf == "or")
            return Token::Or();
        else if (buf == "not")
            return Token::Not();
        else
            return Token::Identifier(buf);
    }

    if (isdigit(temp_c)) {
        return Token::Interger(parseInt());
    }

    throw LexingError(ln,offset,"wierd token found");
}


LexingError::LexingError(int ln, int character, const string& message):line(ln),offset(character),
    msg("LexingError as line "+to_string(ln)+",offset "+to_string(character)+":"+message)
{}

const char* LexingError::what() const noexcept {return msg.c_str();}