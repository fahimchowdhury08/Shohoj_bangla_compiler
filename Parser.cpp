#include "Parser.h"
#include <iostream>
#include <stdexcept>

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

ASTPtr Parser::parse() {
    std::cout << "--- Starting Syntax Analysis ---\n";
    std::cout << "(Semantic checks -- undeclared-variable and type-mismatch detection -- run inline as each statement is parsed.)\n\n";
    auto programRoot = std::make_shared<ASTNode>("Program", "Main");
    hadError = false;

    while (!isAtEnd()) {
        try {
            programRoot->addChild(statement());
        } catch (const std::runtime_error& e) {
            std::cout << "Error: " << e.what() << "\n";
            hadError = true;
            synchronize();
        }
    }
    std::cout << (hadError ? "\nParsing finished with errors.\n" : "\nParsing completed successfully.\n");
    return programRoot;
}

ASTPtr Parser::statement() {
    if (match(TokenType::KEYWORD_IF)) {
        return ifStatement();
    } else if (match(TokenType::KEYWORD_WHILE)) {
        return whileStatement();
    } else if (match({TokenType::DATATYPE_INT, TokenType::DATATYPE_FLOAT})) {
        return declaration(previous());
    } else if (match(TokenType::IDENTIFIER)) {
        return assignment(previous());
    } else {
        throw std::runtime_error("Line " + std::to_string(peek().line) +
                                  ": Unexpected token -> " + peek().lexeme);
    }
}

ASTPtr Parser::whileStatement() {
    auto whileNode = std::make_shared<ASTNode>("WhileLoop", "While");

    ASTPtr condition = comparison();
    whileNode->addChild(condition);

    consume(TokenType::LBRACE, "Expected '{' after while condition.");
    auto loopBlock = std::make_shared<ASTNode>("Block", "LoopBody");
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        loopBlock->addChild(statement());
    }
    consume(TokenType::RBRACE, "Expected '}' after while block.");
    whileNode->addChild(loopBlock);

    return whileNode;
}

ASTPtr Parser::ifStatement() {
    auto ifNode = std::make_shared<ASTNode>("IfStatement", "If");

    ASTPtr condition = comparison();
    ifNode->addChild(condition);

    consume(TokenType::LBRACE, "Expected '{' after if condition.");
    auto thenBranch = std::make_shared<ASTNode>("Block", "Then");
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        thenBranch->addChild(statement());
    }
    consume(TokenType::RBRACE, "Expected '}' after if block.");
    ifNode->addChild(thenBranch);

    if (match(TokenType::KEYWORD_ELSE)) {
        consume(TokenType::LBRACE, "Expected '{' after else keyword.");
        auto elseBranch = std::make_shared<ASTNode>("Block", "Else");
        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            elseBranch->addChild(statement());
        }
        consume(TokenType::RBRACE, "Expected '}' after else block.");
        ifNode->addChild(elseBranch);
    }

    return ifNode;
}

ASTPtr Parser::declaration(const Token& typeToken) {
    Token name = consume(TokenType::IDENTIFIER, "Expected variable name.");
    analyzer.checkRedeclaration(name.lexeme, name.line);
    consume(TokenType::ASSIGN, "Expected '=' after variable name.");
    ASTPtr expressionNode = expression();
    consume(TokenType::TERMINATOR, "Expected '\u0964' at end of statement.");

    analyzer.checkTypeMismatch(typeToken.type, expressionNode, name.line);
    symbolTable.define(name.lexeme, typeToken.type);

    auto assignNode = std::make_shared<ASTNode>("Assignment", name.lexeme);
    assignNode->addChild(expressionNode);
    return assignNode;
}

ASTPtr Parser::assignment(const Token& name) {
    analyzer.checkVariableDeclaration(name.lexeme, name.line);
    consume(TokenType::ASSIGN, "Expected '='.");
    ASTPtr expressionNode = expression();
    consume(TokenType::TERMINATOR, "Expected '\u0964' at end of statement.");

    TokenType declaredType = symbolTable.resolve(name.lexeme);
    analyzer.checkTypeMismatch(declaredType, expressionNode, name.line);

    auto assignNode = std::make_shared<ASTNode>("Assignment", name.lexeme);
    assignNode->addChild(expressionNode);
    return assignNode;
}

ASTPtr Parser::comparison() {
    ASTPtr left = expression();
    while (match({TokenType::GREATER, TokenType::LESS, TokenType::EQUAL_EQUAL})) {
        Token op = previous();
        ASTPtr right = expression();
        auto parent = std::make_shared<ASTNode>("Condition", op.lexeme);
        parent->addChild(left);
        parent->addChild(right);
        left = parent;
    }
    return left;
}

ASTPtr Parser::expression() {
    ASTPtr left = term();
    while (match({TokenType::PLUS, TokenType::MINUS})) {
        Token op = previous();
        ASTPtr right = term();
        auto parent = std::make_shared<ASTNode>("BinaryOp", op.lexeme);
        parent->addChild(left);
        parent->addChild(right);
        left = parent;
    }
    return left;
}

ASTPtr Parser::term() {
    ASTPtr left = factor();
    while (match({TokenType::MULTIPLY, TokenType::DIVIDE})) {
        Token op = previous();
        ASTPtr right = factor();
        auto parent = std::make_shared<ASTNode>("BinaryOp", op.lexeme);
        parent->addChild(left);
        parent->addChild(right);
        left = parent;
    }
    return left;
}

ASTPtr Parser::factor() {
    if (match(TokenType::NUMBER)) {
        return std::make_shared<ASTNode>("Number", previous().lexeme);
    }
    if (match(TokenType::IDENTIFIER)) {
        Token idTok = previous();
        analyzer.checkVariableDeclaration(idTok.lexeme, idTok.line);
        return std::make_shared<ASTNode>("Identifier", idTok.lexeme);
    }
    throw std::runtime_error("Line " + std::to_string(peek().line) +
                              ": Expected number or identifier.");
}

bool Parser::match(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) { advance(); return true; }
    }
    return false;
}

bool Parser::match(TokenType type) {
    return match({type});
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error(message);
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::TOK_EOF;
}

const Token& Parser::peek() const {
    return tokens[current];
}

const Token& Parser::previous() const {
    return tokens[current - 1];
}

void Parser::synchronize() {
    std::cout << "Recovering... Skipping to next statement.\n";
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::TERMINATOR || previous().type == TokenType::RBRACE) return;
        advance();
    }
}
