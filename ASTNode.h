#ifndef ASTNODE_H
#define ASTNODE_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>

class ASTNode {
public:
    std::string nodeType;
    std::string value;
    std::vector<std::shared_ptr<ASTNode>> children;

    ASTNode(std::string nodeType, std::string value)
        : nodeType(std::move(nodeType)), value(std::move(value)) {}

    void addChild(const std::shared_ptr<ASTNode>& child) {
        children.push_back(child);
    }

    // Pretty-prints the tree to stdout, same style as the Java version.
    void printTree(const std::string& prefix, bool isTail) const {
        std::cout << prefix << (isTail ? "\u2514\u2500\u2500 " : "\u251C\u2500\u2500 ")
                  << nodeType << (value.empty() ? "" : " : " + value) << "\n";
        for (size_t i = 0; i + 1 < children.size(); i++) {
            children[i]->printTree(prefix + (isTail ? "    " : "\u2502   "), false);
        }
        if (!children.empty()) {
            children.back()->printTree(prefix + (isTail ? "    " : "\u2502   "), true);
        }
    }
};

using ASTPtr = std::shared_ptr<ASTNode>;

#endif
