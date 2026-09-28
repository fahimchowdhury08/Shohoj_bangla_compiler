#ifndef TOKENTYPE_H
#define TOKENTYPE_H

// All token categories recognized by the Lexer.
enum class TokenType {
    // Keywords: data types
    DATATYPE_INT,     // পূর্ণ
    DATATYPE_FLOAT,   // ভগ্নাংশ

    // Keywords: control flow
    KEYWORD_IF,        // যদি
    KEYWORD_ELSE,       // নয়তো
    KEYWORD_WHILE,      // যতক্ষণ_ধরে

    // Identifiers & literals
    IDENTIFIER,
    NUMBER,

    // Operators
    ASSIGN,        // =
    PLUS, MINUS,        // + -
    MULTIPLY, DIVIDE,      // * /
    GREATER, LESS,        // > <
    EQUAL_EQUAL,        // ==

    // Punctuation
    TERMINATOR,   // । (Bengali danda -- statement end)
    LBRACE, RBRACE,       // { }

    TOK_EOF                 // End of File
};

// Human readable name, used for debug / error printing.
inline const char* tokenTypeName(TokenType t) {
    switch (t) {
        case TokenType::DATATYPE_INT: return "DATATYPE_INT";
        case TokenType::DATATYPE_FLOAT: return "DATATYPE_FLOAT";
        case TokenType::KEYWORD_IF: return "KEYWORD_IF";
        case TokenType::KEYWORD_ELSE: return "KEYWORD_ELSE";
        case TokenType::KEYWORD_WHILE: return "KEYWORD_WHILE";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::ASSIGN: return "ASSIGN";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::MULTIPLY: return "MULTIPLY";
        case TokenType::DIVIDE: return "DIVIDE";
        case TokenType::GREATER: return "GREATER";
        case TokenType::LESS: return "LESS";
        case TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
        case TokenType::TERMINATOR: return "TERMINATOR";
        case TokenType::LBRACE: return "LBRACE";
        case TokenType::RBRACE: return "RBRACE";
        case TokenType::TOK_EOF: return "EOF";
    }
    return "UNKNOWN";
}

#endif
