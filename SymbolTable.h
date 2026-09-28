#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <string>
#include <unordered_map>
#include <sstream>
#include "TokenType.h"

// Tracks each declared variable's data type so the Semantic Analyzer can
// check for undeclared use and type mismatches.
class SymbolTable {
public:
    void define(const std::string& name, TokenType type) {
        table[name] = type;
    }

    bool exists(const std::string& name) const {
        return table.find(name) != table.end();
    }

    TokenType resolve(const std::string& name) const {
        return table.at(name); // caller must check exists() first
    }

    const std::unordered_map<std::string, TokenType>& getTable() const {
        return table;
    }

    std::string toString() const {
        std::ostringstream ss;
        ss << "{";
        bool first = true;
        for (const auto& [name, type] : table) {
            if (!first) ss << ", ";
            ss << name << "=" << tokenTypeName(type);
            first = false;
        }
        ss << "}";
        return ss.str();
    }

private:
    std::unordered_map<std::string, TokenType> table;
};

#endif
