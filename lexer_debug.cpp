//
// Created by cre-chan on 19-11-15.
//

#include <stdafx.h>
#include <lexer.h>

int main() {
    Lexer cmd;
    cout<<">>>";
    do{
        auto input=cmd.getToken();
        if (input.getTag()!=Token::TokenType::nextline)
            std::cout<<input<<" ";
        if (input.getTag()==Token::TokenType::nextline) cout<<input<<">>>";
        if (input.getTag()==Token::TokenType ::eof) break;
    }while(true);

    return 0;
}