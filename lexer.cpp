#include <stdafx.h>
#include <clisp_execption.h>
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
Token::Token(TokenType t, const string &s) : tag(t), content(s) {}

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

Token Token::Rbr() { return Token(TokenType::rbr); }

Token Token::Def() { return Token(TokenType::def); }

Token Token::Cond() { return Token(TokenType::cond); }

Token Token::Identifier(const string &name) { return Token(TokenType::identifier, name); }

Token Token::Interger(int i) { return Token(TokenType::interger, i); }

Token Token::Minus() { return Token(TokenType::minus); }

Token Token::Plus() { return Token(TokenType::plus); }

Token Token::Mul() { return Token(TokenType::mul); }

Token Token::Div() { return Token(TokenType::div); }

Token Token::Eof() { return Token(TokenType::eof); }

Token Token::Not() { return Token(TokenType::not_op); }

Token Token::And() { return Token(TokenType::and_op); }

Token Token::Or() { return Token(TokenType::or_op); }

Token Token::Eq() { return Token(TokenType::eq); }

Token Token::Lt() { return Token(TokenType::lt); }

Token Token::Gt() { return Token(TokenType::gt); }

Token Token::Ge() { return Token(TokenType::ge); }

Token Token::Le() { return Token(TokenType::le); }

Token Token::Boolean(bool b) { return Token(TokenType::boolean, b); }

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
            out << "def_func";
            break;
        case TokenType::lbr:
            out << "LBR";
            break;
        case TokenType::rbr:
            out << "RBR";
            break;
        case TokenType::nextline:
            out << endl;
            break;
        case TokenType::boolean:
            out << "boolean(" << (self.getBooleanValue() ? "true" : "false") << ")";
            break;
        case TokenType::ge:
            out << "ge";
            break;
        case TokenType::or_op:
            out << "or";
            break;
        case TokenType::and_op:
            out << "and";
            break;
        case TokenType::not_op:
            out << "not";
            break;
        case TokenType::gt:
            out << "gt";
            break;
        case TokenType::le:
            out << "le";
            break;
        case TokenType::lt:
            out << "lt";
            break;
        case TokenType::eq:
            out << "eq";
            break;
        case Token::comment:break;
    }

    return out;
}

istream &operator>>(istream &in, Token &token) {

    char look_ahead;

    if (in >> look_ahead) {

        switch (look_ahead) {
            case '(':
                token = Token::Lbr();
                return in;
            case ')':
                token = Token::Rbr();
                return in;
            case '+':
                token = Token::Plus();
                return in;
            case '-':
                if (isdigit(in.peek())) {
                    cin.putback(look_ahead);
                    int temp;
                    in>>temp;
                    token=Token::Interger(temp);
                }else {
                    token = Token::Minus();
                }
                return in;
            case '*':
                token = Token::Mul();
                return in;
            case '/':
                token = Token::Div();
                return in;
            case '=':
                token = Token::Eq();
                return in;
            case '>':
            case '<': {
                cin.putback(look_ahead);
                string op;
                cin >> op;
                if (op == "<")
                    token = Token::Lt();
                else if (op == "<=")
                    token = Token::Le();
                else if (op == ">")
                    token = Token::Gt();
                else if (op == ">=")
                    token = Token::Ge();
                else
                    throw InvalidToken(in, "<, >, >= or <=");

                return in;
            }
            case '#':
                in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                token = Token::Comment();
                return in;
            default:
                if (isdigit(look_ahead)) {
                    cin.putback((look_ahead));
                    int i;
                    cin >> i;
                    token = Token::Interger(i);
                } else if (isalpha((look_ahead)) || look_ahead == '_') {
                    cin.putback((look_ahead));
                    string buf;
                    for (look_ahead = cin.get(); isalnum(look_ahead) || look_ahead == '_'; look_ahead = cin.get()) {
                        if (look_ahead) buf.push_back(look_ahead);
                    }

                    cin.putback(look_ahead);

                    if (buf == "define")
                        token = Token::Def();
                    else if (buf == "if")
                        token = Token::Cond();
                    else if (buf == "true")
                        token = Token::Boolean(true);
                    else if (buf == "false")
                        token = Token::Boolean(false);
                    else if (buf == "and")
                        token = Token::And();
                    else if (buf == "or")
                        token = Token::Or();
                    else if (buf == "not")
                        token = Token::Not();
                    else
                        token = Token::Identifier(buf);

                } else {
                    //当读取到一个非数字，下划线，字母的符号时，报告不可识别字符错误
                    throw UnrecognizedCharacter(look_ahead, in);
                }
                return in;

        }
    }

    token = Token::Eof();
    return in;
}


Token::Token() = default;

#pragma clang diagnostic pop

Token Token::Default() {
    return Token();
}

Token Token::Comment() {
    return Token(comment);
}


