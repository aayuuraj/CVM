#pragma once

#include "token.h"
#include "ast.h"
#include <vector>
#include <memory>

// Recursive descent parser: converts a token stream into an AST
class Parser {
public:
    // Construct a parser for the given token sequence
    explicit Parser(const std::vector<Token>& tokens);

    // Parse the entire token stream into a ProgramNode AST
    std::unique_ptr<ProgramNode> parse();

private:
    std::vector<Token> tokens_;
    size_t current_;

    // --- Token navigation helpers ---

    // Return the current token without advancing
    const Token& peek() const;

    // Return the previous token
    const Token& previous() const;

    // Check if we've reached the end of tokens
    bool isAtEnd() const;

    // Advance and return the consumed token
    const Token& advance();

    // Check if the current token matches a given type
    bool check(TokenType type) const;

    // If the current token matches any of the given types, advance and return true
    bool match(std::initializer_list<TokenType> types);

    // Consume a token of the expected type, or throw an error
    const Token& consume(TokenType type, const std::string& errMsg);

    // Throw a parse error with location info
    [[noreturn]] void error(const std::string& msg) const;

    // --- Grammar rules ---

    // Parse a single statement
    ASTNodePtr parseStatement();

    // Parse a let declaration
    ASTNodePtr parseLetStatement();

    // Parse a print statement
    ASTNodePtr parsePrintStatement();

    // Parse an input statement
    ASTNodePtr parseInputStatement();

    // Parse an if-else statement
    ASTNodePtr parseIfStatement();

    // Parse a while loop
    ASTNodePtr parseWhileStatement();

    // Parse a { ... } block of statements
    ASTNodePtr parseBlock();

    // Parse an expression-statement or assignment
    ASTNodePtr parseExpressionStatement();

    // Parse an expression (entry point for expression parsing)
    ASTNodePtr parseExpression();

    // Parse equality operators (==)
    ASTNodePtr parseEquality();

    // Parse comparison operators (<)
    ASTNodePtr parseComparison();

    // Parse additive operators (+ -)
    ASTNodePtr parseAddition();

    // Parse multiplicative operators (* /)
    ASTNodePtr parseMultiplication();

    // Parse unary operators (currently just -)
    ASTNodePtr parseUnary();

    // Parse primary expressions (literals, identifiers, grouped expressions)
    ASTNodePtr parsePrimary();
};
