#include <utility>

//
// Created by cre-chan on 19-12-16.
//

#include "stdafx.h"
#include "clisp_execption.h"

UnrecognizedCharacter::UnrecognizedCharacter(char c, istream &in) :
        ClispError(in), invalid_char(c) {
}

const string UnrecognizedCharacter::info() const noexcept {
    return string("Unrecognizable character ") + invalid_char + " encountered.";
}

ClispError::ClispError(istream &in) :
        exception() {
    in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

InvalidToken::InvalidToken(istream &istream, string tok) : ClispError(istream), tok_name(std::move(tok)) {}

const string InvalidToken::info() const noexcept {
    return "Invalid token found. Did you mean "+tok_name+"?";
}

UnmatchingBracket::UnmatchingBracket(istream &istream) : ClispError(istream) {}

const string UnmatchingBracket::info() const noexcept {
    return "More enclosing brackets than opening brackets.";
}

PrimaryIsNotCallable::PrimaryIsNotCallable(istream &istream, unique_ptr<Expr> expr) : ClispError(istream),
                                                                                             expr(std::move(expr)) {}

const string PrimaryIsNotCallable::info() const noexcept {
    return "Primary expression cannot be applied to arguments.";
}

ExpectingExpr::ExpectingExpr(istream &istream) : ClispError(istream) {}

const string ExpectingExpr::info() const noexcept {
    return "Expecting expression while a token is found.";
}

EmptyExpr::EmptyExpr(istream &istream) : ClispError(istream) {}

const string EmptyExpr::info() const noexcept {
    return "Empty expression enclosed by brackets.";
}

ExpectingOperands::ExpectingOperands(istream &istream, uint argNum) : ClispError(istream), arg_num(argNum) {}

const string ExpectingOperands::info() const noexcept {
    return "Not enough operands or too many operands found.";
}

ExpectingIdOrArglist::ExpectingIdOrArglist(istream &istream) : ClispError(istream) {}

const string ExpectingIdOrArglist::info() const noexcept {
    return "Expecting identifier or argument list.";
}
