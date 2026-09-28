#include <iostream>
#include <fstream>
#include <sstream>
#include "Lexer.h"
#include "Parser.h"
#include "TACGenerator.h"
#include "CodeGenerator.h"

#ifdef _WIN32
#include <windows.h>
#endif

static std::string readFile(const std::string& path, bool& ok) {
    std::ifstream file(path, std::ios::binary);
    if (!file) { ok = false; return ""; }
    std::ostringstream ss;
    ss << file.rdbuf();
    ok = true;
    return ss.str();
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    std::string filePath = (argc > 1) ? argv[1] : "input.bng";
    std::string outputFilePath = (argc > 2) ? argv[2] : "output.py";

    bool ok;
    std::string sourceCode = readFile(filePath, ok);
    if (!ok) {
        std::cout << "ERROR: Could not find '" << filePath << "'.\n";
        return 1;
    }

    std::cout << "=== BANGLA COMPILER (C++) ===\n\n";
    std::cout << "Reading from: " << filePath << "\n\n";
    std::cout << "--- Source Code ---\n" << sourceCode << "\n";
    std::cout << "-------------------\n\n";

    std::vector<Token> tokens;
    try {
        // 1. Lexical Analysis
        Lexer lexer(sourceCode);
        tokens = lexer.scanTokens();
    } catch (const std::exception& e) {
        // Malformed/non-UTF-8 input should never crash the compiler.
        std::cout << "ERROR: Could not read source as valid UTF-8 text: " << e.what() << "\n";
        return 1;
    }

    std::cout << "--- Tokens ---\n";
    for (const Token& t : tokens) {
        std::cout << "Line " << t.line << ": " << t.toString() << "\n";
    }
    std::cout << "\n";

    // 2. Syntax Analysis (building the AST; semantic checks run inline -- see below)
    Parser parser(tokens);
    ASTPtr root = parser.parse();

    // 2a. Symbol Table -- populated as declarations are parsed
    std::cout << "\n--- Symbol Table ---\n";
    const auto& table = parser.getSymbolTable().getTable();
    if (table.empty()) {
        std::cout << "(empty)\n";
    } else {
        std::cout << "Name\tType\n";
        std::cout << "----\t----\n";
        for (const auto& [name, type] : table) {
            std::cout << name << "\t" << tokenTypeName(type) << "\n";
        }
    }

    // 2b. Semantic Analysis summary -- undeclared-variable & type-mismatch
    // checks already ran inline while parsing (see Parser::declaration /
    // Parser::assignment / Parser::factor, which call into SemanticAnalyzer).
    std::cout << "\n--- Semantic Analysis ---\n";
    if (parser.hasError()) {
        std::cout << "Semantic/syntax errors were found above; see messages for details.\n";
    } else {
        std::cout << "No undeclared-variable or type-mismatch errors found.\n";
        std::cout << "All " << table.size() << " declared variable(s) type-checked successfully.\n";
    }

    // 3. AST
    std::cout << "\n--- Abstract Syntax Tree (AST) ---\n";
    if (root) {
        root->printTree("", true);
    }

    // 4. Intermediate Representation: Three-Address Code
    std::cout << "\n--- Three-Address Code (Intermediate Representation) ---\n";
    if (root) {
        TACGenerator tacGen;
        tacGen.generate(root);
        tacGen.print();
    }

    // 5. Code Generation (target output: Python)
    std::cout << "\n";
    if (root) {
        CodeGenerator generator;
        generator.generate(root, outputFilePath);

        std::cout << "\n--- Generated Python Code (Target Output) ---\n";
        std::cout << generator.getGeneratedCode();
    }

    return 0;
}