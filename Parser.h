#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>
#include "Token.h"
#include "ASTNode.h"
#include "SymbolTable.h"
#include "SemanticAnalyzer.h"

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    ASTPtr parse();
    const SymbolTable& getSymbolTable() const { return symbolTable; }
    bool hasError() const { return hadError; }

private:
    std::vector<Token> tokens;
    size_t current = 0;
    bool hadError = false;
    SymbolTable symbolTable;
    SemanticAnalyzer analyzer{symbolTable};

    ASTPtr statement();
    ASTPtr ifStatement();
    ASTPtr whileStatement();
    ASTPtr declaration(const Token& typeToken);
    ASTPtr assignment(const Token& name);

    ASTPtr comparison();
    ASTPtr expression();
    ASTPtr term();
    ASTPtr factor();

    bool match(std::initializer_list<TokenType> types);
    bool match(TokenType type);
    Token consume(TokenType type, const std::string& message);
    bool check(TokenType type) const;
    Token advance();
    bool isAtEnd() const;
    const Token& peek() const;
    const Token& previous() const;
    void synchronize();
};

#endif
