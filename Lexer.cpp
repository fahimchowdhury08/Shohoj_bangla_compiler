#include "Lexer.h"
#include "Utf8Utils.h"
#include <iostream>

// ---- Keyword table for this language ------------------------------------
// পূর্ণ        -> integer type
// ভগ্নাংশ      -> decimal type
// যদি          -> if
// নয়তো        -> else
// যতক্ষণ_ধরে   -> while
// (statement terminator is the Bengali "।" danda, handled in scanToken)
namespace {
    const std::u32string KW_INT      = utf8Decode("পূর্ণ");
    const std::u32string KW_FLOAT    = utf8Decode("ভগ্নাংশ");
    const std::u32string KW_IF       = utf8Decode("যদি");
    const std::u32string KW_ELSE     = utf8Decode("নয়তো");
    const std::u32string KW_WHILE    = utf8Decode("যতক্ষণ_ধরে");
    const char32_t TERMINATOR_CP     = U'।'; // Bengali danda, U+0964
}

Lexer::Lexer(const std::string& utf8Source) {
    source = utf8Decode(utf8Source);
}

std::vector<Token> Lexer::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }
    tokens.emplace_back(TokenType::TOK_EOF, "", line);
    return tokens;
}

void Lexer::scanToken() {
    char32_t c = advance();

    switch (c) {
        case U'=': addToken(match(U'=') ? TokenType::EQUAL_EQUAL : TokenType::ASSIGN); break;
        case U'+': addToken(TokenType::PLUS); break;
        case U'-': addToken(TokenType::MINUS); break;
        case U'*': addToken(TokenType::MULTIPLY); break;
        case U'/': addToken(TokenType::DIVIDE); break;
        case U'{': addToken(TokenType::LBRACE); break;
        case U'}': addToken(TokenType::RBRACE); break;
        case U'>': addToken(TokenType::GREATER); break;
        case U'<': addToken(TokenType::LESS); break;

        case U' ':
        case U'\r':
        case U'\t':
            break;

        case U'\n':
            line++;
            break;

        default:
            if (c == TERMINATOR_CP) {
                addToken(TokenType::TERMINATOR);
            } else if (isBanglaDigit(c)) {
                number();
            } else if (isAlpha(c)) {
                identifier();
            } else {
                std::cerr << "Line " << line << ": Unexpected character '"
                          << utf8Encode(c) << "'\n";
            }
            break;
    }
}

void Lexer::identifier() {
    while (isAlpha(peek()) || isBanglaDigit(peek())) {
        advance();
    }

    std::u32string text = source.substr(start, current - start);

    if (text == KW_INT) {
        addToken(TokenType::DATATYPE_INT);
    } else if (text == KW_FLOAT) {
        addToken(TokenType::DATATYPE_FLOAT);
    } else if (text == KW_IF) {
        addToken(TokenType::KEYWORD_IF);
    } else if (text == KW_ELSE) {
        addToken(TokenType::KEYWORD_ELSE);
    } else if (text == KW_WHILE) {
        addToken(TokenType::KEYWORD_WHILE);
    } else {
        addToken(TokenType::IDENTIFIER);
    }
}

void Lexer::number() {
    while (isBanglaDigit(peek())) {
        advance();
    }

    if (peek() == U'.' && isBanglaDigit(peekNext())) {
        advance();
        while (isBanglaDigit(peek())) {
            advance();
        }
    }

    std::u32string raw = source.substr(start, current - start);
    std::u32string converted;
    for (char32_t c : raw) {
        if (c >= 0x09E6 && c <= 0x09EF) {
            converted.push_back(U'0' + (c - 0x09E6)); // Bengali digit -> ASCII digit
        } else {
            converted.push_back(c); // the '.' in a decimal literal
        }
    }
    tokens.emplace_back(TokenType::NUMBER, utf8Encode(converted), line);
}

bool Lexer::match(char32_t expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    return true;
}

bool Lexer::isBanglaDigit(char32_t c) const {
    return c >= 0x09E6 && c <= 0x09EF;
}

bool Lexer::isAlpha(char32_t c) const {
    return (c >= 0x0980 && c <= 0x09FF)              // Bengali Unicode block
           || (c >= U'a' && c <= U'z')
           || (c >= U'A' && c <= U'Z')
           || c == U'_';
}

char32_t Lexer::advance() {
    return source[current++];
}

char32_t Lexer::peek() const {
    return isAtEnd() ? U'\0' : source[current];
}

char32_t Lexer::peekNext() const {
    return (current + 1 >= source.size()) ? U'\0' : source[current + 1];
}

bool Lexer::isAtEnd() const {
    return current >= source.size();
}

void Lexer::addToken(TokenType type) {
    std::u32string lexemeCp = source.substr(start, current - start);
    tokens.emplace_back(type, utf8Encode(lexemeCp), line);
}
