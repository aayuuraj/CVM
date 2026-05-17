#pragma once

#include "token.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Forward declarations for the Visitor pattern
struct ASTVisitor;

// ============================================================================
// AST Node Base
// ============================================================================

// Base class for all AST nodes
struct ASTNode {
    virtual ~ASTNode() = default;
    // Pretty-print the node for debug output
    virtual void dump(int indent = 0) const = 0;
};

using ASTNodePtr = std::unique_ptr<ASTNode>;

// ============================================================================
// Expression Nodes
// ============================================================================

// Integer literal (e.g., 42)
struct NumberLiteralNode : ASTNode {
    int64_t value;

    explicit NumberLiteralNode(int64_t value) : value(value) {}

    void dump(int indent) const override {
        std::string pad(indent * 2, ' ');
        std::cout << pad << "NumberLiteral(" << value << ")\n";
    }
};

// Boolean literal (true / false)
struct BoolLiteralNode : ASTNode {
    bool value;

    explicit BoolLiteralNode(bool value) : value(value) {}

    void dump(int indent) const override {
        std::string pad(indent * 2, ' ');
        std::cout << pad << "BoolLiteral(" << (value ? "true" : "false") << ")\n";
    }
};

// Variable reference (e.g., x)
struct IdentifierNode : ASTNode {
    std::string name;

    explicit IdentifierNode(std::string name) : name(std::move(name)) {}

    void dump(int indent) const override {
        std::string pad(indent * 2, ' ');
        std::cout << pad << "Identifier(" << name << ")\n";
    }
};

// Binary operation (e.g., x + 5, a == b)
struct BinaryOpNode : ASTNode {
    ASTNodePtr left;
    TokenType op;
    ASTNodePtr right;

    BinaryOpNode(ASTNodePtr left, TokenType op, ASTNodePtr right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    void dump(int indent) const override {
        std::string pad(indent * 2, ' ');
        std::cout << pad << "BinaryOp(" << tokenTypeName(op) << ")\n";
        left->dump(indent + 1);
        right->dump(indent + 1);
    }
};

// ============================================================================
// Statement Nodes
// ============================================================================

// Variable declaration (let x = expr;)
struct LetNode : ASTNode {
    std::string name;
    ASTNodePtr initializer;

    LetNode(std::string name, ASTNodePtr init)
        : name(std::move(name)), initializer(std::move(init)) {}

    void dump(int indent) const override {
        std::string pad(indent * 2, ' ');
        std::cout << pad << "LetNode(" << name << ")\n";
        initializer->dump(indent + 1);
    }
};

// Assignment (x = expr;)
struct AssignNode : ASTNode {
    std::string name;
    ASTNodePtr value;

    AssignNode(std::string name, ASTNodePtr value)
        : name(std::move(name)), value(std::move(value)) {}

    void dump(int indent) const override {
        std::string pad(indent * 2, ' ');
        std::cout << pad << "AssignNode(" << name << ")\n";
        value->dump(indent + 1);
    }
};

// Print statement (print expr;)
struct PrintNode : ASTNode {
    ASTNodePtr expression;

    explicit PrintNode(ASTNodePtr expr) : expression(std::move(expr)) {}

    void dump(int indent) const override {
        std::string pad(indent * 2, ' ');
        std::cout << pad << "PrintNode\n";
        expression->dump(indent + 1);
    }
};

// Input statement (input x;)
struct InputNode : ASTNode {
    std::string variableName;

    explicit InputNode(std::string name) : variableName(std::move(name)) {}

    void dump(int indent) const override {
        std::string pad(indent * 2, ' ');
        std::cout << pad << "InputNode(" << variableName << ")\n";
    }
};

// Block of statements ({ ... })
struct BlockNode : ASTNode {
    std::vector<ASTNodePtr> statements;

    explicit BlockNode(std::vector<ASTNodePtr> stmts)
        : statements(std::move(stmts)) {}

    void dump(int indent) const override {
        std::string pad(indent * 2, ' ');
        std::cout << pad << "BlockNode\n";
        for (auto& s : statements) {
            s->dump(indent + 1);
        }
    }
};

// If-else statement
struct IfNode : ASTNode {
    ASTNodePtr condition;
    ASTNodePtr thenBlock;
    ASTNodePtr elseBlock;  // May be nullptr

    IfNode(ASTNodePtr cond, ASTNodePtr thenB, ASTNodePtr elseB = nullptr)
        : condition(std::move(cond)),
          thenBlock(std::move(thenB)),
          elseBlock(std::move(elseB)) {}

    void dump(int indent) const override {
        std::string pad(indent * 2, ' ');
        std::cout << pad << "IfNode\n";
        std::cout << pad << "  Condition:\n";
        condition->dump(indent + 2);
        std::cout << pad << "  Then:\n";
        thenBlock->dump(indent + 2);
        if (elseBlock) {
            std::cout << pad << "  Else:\n";
            elseBlock->dump(indent + 2);
        }
    }
};

// While loop
struct WhileNode : ASTNode {
    ASTNodePtr condition;
    ASTNodePtr body;

    WhileNode(ASTNodePtr cond, ASTNodePtr body)
        : condition(std::move(cond)), body(std::move(body)) {}

    void dump(int indent) const override {
        std::string pad(indent * 2, ' ');
        std::cout << pad << "WhileNode\n";
        std::cout << pad << "  Condition:\n";
        condition->dump(indent + 2);
        std::cout << pad << "  Body:\n";
        body->dump(indent + 2);
    }
};

// Root node: the entire program
struct ProgramNode : ASTNode {
    std::vector<ASTNodePtr> statements;

    explicit ProgramNode(std::vector<ASTNodePtr> stmts)
        : statements(std::move(stmts)) {}

    void dump(int indent) const override {
        std::string pad(indent * 2, ' ');
        std::cout << pad << "ProgramNode\n";
        for (auto& s : statements) {
            s->dump(indent + 1);
        }
    }
};
