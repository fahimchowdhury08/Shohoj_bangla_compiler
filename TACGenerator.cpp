#include "TACGenerator.h"
#include <iostream>

std::string TACGenerator::newTemp() {
    return "t" + std::to_string(++tempCount);
}

std::string TACGenerator::newLabel() {
    return "L" + std::to_string(++labelCount);
}

const std::vector<std::string>& TACGenerator::generate(const ASTPtr& root) {
    instructions.clear();
    tempCount = 0;
    labelCount = 0;
    genStmt(root);
    return instructions;
}

std::string TACGenerator::genExpr(const ASTPtr& node) {
    if (!node) return "";

    if (node->nodeType == "Number" || node->nodeType == "Identifier") {
        return node->value;
    }

    if (node->nodeType == "BinaryOp" || node->nodeType == "Condition") {
        std::string left = genExpr(node->children[0]);
        std::string right = genExpr(node->children[1]);
        std::string temp = newTemp();
        instructions.push_back(temp + " = " + left + " " + node->value + " " + right);
        return temp;
    }

    return "";
}

void TACGenerator::genStmt(const ASTPtr& node) {
    if (!node) return;
    const std::string& t = node->nodeType;

    if (t == "Program" || t == "Block") {
        for (const auto& child : node->children) genStmt(child);

    } else if (t == "Assignment") {
        if (node->children.empty()) return;
        std::string result = genExpr(node->children[0]);
        instructions.push_back(node->value + " = " + result);

    } else if (t == "IfStatement") {
        if (node->children.size() < 2) return;
        std::string condPlace = genExpr(node->children[0]);
        bool hasElse = node->children.size() > 2;
        std::string elseLabel = newLabel();
        std::string endLabel = hasElse ? newLabel() : elseLabel;

        instructions.push_back("ifFalse " + condPlace + " goto " + elseLabel);
        genStmt(node->children[1]); // then-block
        if (hasElse) {
            instructions.push_back("goto " + endLabel);
            instructions.push_back(elseLabel + ":");
            genStmt(node->children[2]); // else-block
        }
        instructions.push_back(endLabel + ":");

    } else if (t == "WhileLoop") {
        if (node->children.size() < 2) return;
        std::string startLabel = newLabel();
        std::string endLabel = newLabel();

        instructions.push_back(startLabel + ":");
        std::string condPlace = genExpr(node->children[0]);
        instructions.push_back("ifFalse " + condPlace + " goto " + endLabel);
        genStmt(node->children[1]); // loop body block
        instructions.push_back("goto " + startLabel);
        instructions.push_back(endLabel + ":");
    }
}

void TACGenerator::print() const {
    for (const std::string& instr : instructions) {
        // Labels (lines ending in ':') sit flush left; code under them is indented.
        if (!instr.empty() && instr.back() == ':') {
            std::cout << instr << "\n";
        } else {
            std::cout << "    " << instr << "\n";
        }
    }
}
