#ifndef SEMANTICANALYZER_H
#define SEMANTICANALYZER_H

#include <string>
#include <stdexcept>
#include "TokenType.h"
#include "SymbolTable.h"
#include "ASTNode.h"

// Performs the two type-checking duties required by the project spec:
//   1. Undeclared-variable use
//   2. Type mismatch (assigning a decimal literal to an integer variable)
class SemanticAnalyzer {
public:
    explicit SemanticAnalyzer(SymbolTable& symbolTable) : symbolTable(symbolTable) {}

    void checkVariableDeclaration(const std::string& varName, int line) const {
        if (!symbolTable.exists(varName)) {
            throw std::runtime_error("Line " + std::to_string(line) +
                                      ": Undeclared variable '" + varName + "'.");
        }
    }

    void checkRedeclaration(const std::string& varName, int line) const {
        if (symbolTable.exists(varName)) {
            throw std::runtime_error("Line " + std::to_string(line) +
                                      ": Variable '" + varName + "' is already declared.");
        }
    }

    // Only literal decimal values can be checked statically in this toy
    // language (no full type inference over arbitrary expressions), so this
    // looks at the initializer node directly: if it's a bare Number literal
    // containing '.', and the declared type is the integer type, that's a
    // mismatch.
    void checkTypeMismatch(TokenType declaredType, const ASTPtr& initializer, int line) const {
        if (declaredType != TokenType::DATATYPE_INT) return;
        if (!initializer || initializer->nodeType != "Number") return;
        if (initializer->value.find('.') != std::string::npos) {
            throw std::runtime_error("Line " + std::to_string(line) +
                                      ": Type Mismatch! Cannot assign a decimal value to '\u09aa\u09c2\u09b0\u09cd\u09a3'.");
        }
    }

private:
    SymbolTable& symbolTable;
};

#endif
