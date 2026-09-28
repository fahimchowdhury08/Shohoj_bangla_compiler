#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include <string>
#include "ASTNode.h"

// Walks the AST and emits runnable Python 3 source.
class CodeGenerator {
public:
    CodeGenerator();
    void generate(const ASTPtr& root, const std::string& outputFileName);
    const std::string& getGeneratedCode() const { return pythonCode; }

private:
    std::string pythonCode;
    int indentLevel = 0;

    void writeIndent();
    void traverse(const ASTPtr& node);
    bool writeToFile(const std::string& filename) const;
};

#endif
