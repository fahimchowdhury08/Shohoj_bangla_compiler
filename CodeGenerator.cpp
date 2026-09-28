#include "CodeGenerator.h"
#include <fstream>
#include <iostream>

CodeGenerator::CodeGenerator() {
    pythonCode = "#Auto-Generated Python Code from Bangla Compiler\n\n";
}

void CodeGenerator::generate(const ASTPtr& root, const std::string& outputFileName) {
    if (!root) return; // defensive: never crash on a null tree
    std::cout << "--- Starting Code Generation ---\n";
    traverse(root);
    writeToFile(outputFileName);
}

void CodeGenerator::writeIndent() {
    for (int i = 0; i < indentLevel; i++) pythonCode += "    ";
}

void CodeGenerator::traverse(const ASTPtr& node) {
    if (!node) return; // defensive guard, should not happen from a well-formed parse

    const std::string& t = node->nodeType;

    if (t == "Program") {
        for (const auto& child : node->children) traverse(child);

    } else if (t == "IfStatement") {
        if (node->children.size() < 2) { pythonCode += "# malformed if-statement\n"; return; }
        writeIndent();
        pythonCode += "if ";
        traverse(node->children[0]);
        pythonCode += ":\n";
        traverse(node->children[1]);
        if (node->children.size() > 2) {
            writeIndent();
            pythonCode += "else:\n";
            traverse(node->children[2]);
        }

    } else if (t == "WhileLoop") {
        if (node->children.size() < 2) { pythonCode += "# malformed while-loop\n"; return; }
        writeIndent();
        pythonCode += "while ";
        traverse(node->children[0]);
        pythonCode += ":\n";
        traverse(node->children[1]);

    } else if (t == "Block") {
        indentLevel++;
        if (node->children.empty()) {
            // Python has no empty blocks -- keep the generated file valid.
            writeIndent();
            pythonCode += "pass\n";
        } else {
            for (const auto& child : node->children) traverse(child);
        }
        indentLevel--;

    } else if (t == "Assignment") {
        if (node->children.empty()) { pythonCode += "# malformed assignment\n"; return; }
        writeIndent();
        pythonCode += node->value + " = ";
        traverse(node->children[0]);
        pythonCode += "\n";

        writeIndent();
        pythonCode += "print('" + node->value + " =', " + node->value + ")\n";

    } else if (t == "Condition" || t == "BinaryOp") {
        if (node->children.size() < 2) { pythonCode += "# malformed expression"; return; }
        traverse(node->children[0]);
        pythonCode += " " + node->value + " ";
        traverse(node->children[1]);

    } else if (t == "Number" || t == "Identifier") {
        pythonCode += node->value;
    }
}

bool CodeGenerator::writeToFile(const std::string& filename) const {
    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        std::cout << "Error writing generated code: could not open '" << filename << "'\n";
        return false;
    }
    out << pythonCode;
    if (!out) {
        std::cout << "Error writing generated code: write failed for '" << filename << "'\n";
        return false;
    }
    std::cout << "Success: Compiled Python code written to '" << filename << "'\n";
    return true;
}
