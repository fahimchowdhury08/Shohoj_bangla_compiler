#ifndef TACGENERATOR_H
#define TACGENERATOR_H

#include <string>
#include <vector>
#include "ASTNode.h"

// Walks the AST and emits a linear list of three-address-code (TAC)
// instructions -- the classic compiler intermediate representation that sits
// between the AST and target code generation. Every binary operation and
// comparison is broken down into a single operation writing into a fresh
// temporary (t1, t2, ...); if/while control flow is lowered into explicit
// conditional jumps and labels (L1, L2, ...).
class TACGenerator {
public:
    // Generates the instruction list for the given AST and returns it.
    const std::vector<std::string>& generate(const ASTPtr& root);

    // Prints the generated instructions, one per line, with simple
    // indentation so labels stand out from the code under them.
    void print() const;

private:
    std::vector<std::string> instructions;
    int tempCount = 0;
    int labelCount = 0;

    std::string newTemp();
    std::string newLabel();

    // Emits instructions for an expression/condition subtree and returns the
    // "place" (a temp name, identifier, or literal) holding its value.
    std::string genExpr(const ASTPtr& node);

    // Emits instructions for a statement/block/control-flow subtree.
    void genStmt(const ASTPtr& node);
};

#endif
