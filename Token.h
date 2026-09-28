#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include "TokenType.h"

// A single lexical token. lexeme is stored as a UTF-8 encoded std::string
// so it prints correctly and can be written straight to output files.
struct Token {
    TokenType type;
    std::string lexeme;
    int line;

    Token(TokenType type, std::string lexeme, int line)
        : type(type), lexeme(std::move(lexeme)), line(line) {}

    std::string toString() const {
        return std::string(tokenTypeName(type)) + " '" + lexeme + "'";
    }
};

#endif
