#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include "Token.h"

class Lexer {
public:
    explicit Lexer(const std::string& utf8Source);
    std::vector<Token> scanTokens();

private:
    std::u32string source;   // decoded codepoints of the whole file
    std::vector<Token> tokens;
    size_t start = 0;
    size_t current = 0;
    int line = 1;

    void scanToken();
    void identifier();
    void number();

    bool match(char32_t expected);
    bool isBanglaDigit(char32_t c) const;
    bool isAlpha(char32_t c) const;
    char32_t advance();
    char32_t peek() const;
    char32_t peekNext() const;
    bool isAtEnd() const;
    void addToken(TokenType type);
};

#endif
