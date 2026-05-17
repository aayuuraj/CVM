#include "parser.h"
#include <stdexcept>
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens)
    : tokens_(tokens), current_(0) {}

// ============================================================================
// Token navigation helpers
// ============================================================================

const Token& Parser::peek() const {
    return tokens_[current_];
}

const Token& Parser::previous() const {
    return tokens_[current_ - 1];
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EOF_TOKEN;
}

const Token& Parser::advance() {
    if (!isAtEnd()) current_++;
    return previous();
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(std::initializer_list<TokenType> types) {
    for (auto t : types) {
        if (check(t)) {
            advance();
            return true;
        }
    }
    return false;
}

const Token& Parser::consume(TokenType type, const std::string& errMsg) {
    if (check(type)) return advance();
    error(errMsg);
}

void Parser::error(const std::string& msg) const {
    const Token& tok = peek();
    throw std::runtime_error(
        "Parse error at line " + std::to_string(tok.line) +
        ", col " + std::to_string(tok.col) + ": " + msg +
        " (got " + tokenTypeName(tok.type) + ")");
}

// ============================================================================
// Top-level parse
// ============================================================================

std::unique_ptr<ProgramNode> Parser::parse() {
    std::vector<ASTNodePtr> statements;
    while (!isAtEnd()) {
        statements.push_back(parseStatement());
    }
    return std::make_unique<ProgramNode>(std::move(statements));
}

// ============================================================================
// Statement parsing
// ============================================================================

ASTNodePtr Parser::parseStatement() {
    if (check(TokenType::LET))   return parseLetStatement();
    if (check(TokenType::PRINT)) return parsePrintStatement();
    if (check(TokenType::INPUT)) return parseInputStatement();
    if (check(TokenType::IF))    return parseIfStatement();
    if (check(TokenType::WHILE)) return parseWhileStatement();
    if (check(TokenType::LBRACE)) return parseBlock();
    return parseExpressionStatement();
}

// Parse: let <name> = <expr>;
ASTNodePtr Parser::parseLetStatement() {
    consume(TokenType::LET, "Expected 'let'");
    const Token& name = consume(TokenType::IDENTIFIER, "Expected variable name after 'let'");
    consume(TokenType::EQUAL, "Expected '=' after variable name");
    auto init = parseExpression();
    consume(TokenType::SEMICOLON, "Expected ';' after let declaration");
    return std::make_unique<LetNode>(name.value, std::move(init));
}

// Parse: print <expr>;
ASTNodePtr Parser::parsePrintStatement() {
    consume(TokenType::PRINT, "Expected 'print'");
    auto expr = parseExpression();
    consume(TokenType::SEMICOLON, "Expected ';' after print statement");
    return std::make_unique<PrintNode>(std::move(expr));
}

// Parse: input <identifier>;
ASTNodePtr Parser::parseInputStatement() {
    consume(TokenType::INPUT, "Expected 'input'");
    const Token& name = consume(TokenType::IDENTIFIER, "Expected variable name after 'input'");
    consume(TokenType::SEMICOLON, "Expected ';' after input statement");
    return std::make_unique<InputNode>(name.value);
}

// Parse: if (<cond>) { ... } else { ... }
ASTNodePtr Parser::parseIfStatement() {
    consume(TokenType::IF, "Expected 'if'");
    consume(TokenType::LPAREN, "Expected '(' after 'if'");
    auto condition = parseExpression();
    consume(TokenType::RPAREN, "Expected ')' after if condition");
    auto thenBlock = parseBlock();

    ASTNodePtr elseBlock = nullptr;
    if (match({TokenType::ELSE})) {
        elseBlock = parseBlock();
    }

    return std::make_unique<IfNode>(
        std::move(condition), std::move(thenBlock), std::move(elseBlock));
}

// Parse: while (<cond>) { ... }
ASTNodePtr Parser::parseWhileStatement() {
    consume(TokenType::WHILE, "Expected 'while'");
    consume(TokenType::LPAREN, "Expected '(' after 'while'");
    auto condition = parseExpression();
    consume(TokenType::RPAREN, "Expected ')' after while condition");
    auto body = parseBlock();
    return std::make_unique<WhileNode>(std::move(condition), std::move(body));
}

// Parse: { stmt* }
ASTNodePtr Parser::parseBlock() {
    consume(TokenType::LBRACE, "Expected '{'");
    std::vector<ASTNodePtr> stmts;
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        stmts.push_back(parseStatement());
    }
    consume(TokenType::RBRACE, "Expected '}'");
    return std::make_unique<BlockNode>(std::move(stmts));
}

// Parse an expression statement or an assignment (x = expr;)
ASTNodePtr Parser::parseExpressionStatement() {
    // Check for assignment: identifier followed by '='
    if (check(TokenType::IDENTIFIER) &&
        current_ + 1 < tokens_.size() &&
        tokens_[current_ + 1].type == TokenType::EQUAL) {
        std::string name = advance().value;  // consume identifier
        advance();                            // consume '='
        auto value = parseExpression();
        consume(TokenType::SEMICOLON, "Expected ';' after assignment");
        return std::make_unique<AssignNode>(name, std::move(value));
    }

    auto expr = parseExpression();
    consume(TokenType::SEMICOLON, "Expected ';' after expression");
    // Wrap in a print? No — just discard. This is an expression statement.
    // For simplicity, we return the expression itself. The compiler can emit POP.
    return expr;
}

// ============================================================================
// Expression parsing (precedence climbing)
// ============================================================================

ASTNodePtr Parser::parseExpression() {
    return parseEquality();
}

// Equality: comparison (== comparison)*
ASTNodePtr Parser::parseEquality() {
    auto left = parseComparison();
    while (match({TokenType::EQUAL_EQUAL})) {
        TokenType op = previous().type;
        auto right = parseComparison();
        left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    }
    return left;
}

// Comparison: addition (< addition)*
ASTNodePtr Parser::parseComparison() {
    auto left = parseAddition();
    while (match({TokenType::LESS})) {
        TokenType op = previous().type;
        auto right = parseAddition();
        left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    }
    return left;
}

// Addition: multiplication ((+ | -) multiplication)*
ASTNodePtr Parser::parseAddition() {
    auto left = parseMultiplication();
    while (match({TokenType::PLUS, TokenType::MINUS})) {
        TokenType op = previous().type;
        auto right = parseMultiplication();
        left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    }
    return left;
}

// Multiplication: unary ((* | /) unary)*
ASTNodePtr Parser::parseMultiplication() {
    auto left = parseUnary();
    while (match({TokenType::STAR, TokenType::SLASH})) {
        TokenType op = previous().type;
        auto right = parseUnary();
        left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    }
    return left;
}

// Unary: (-) unary | primary
ASTNodePtr Parser::parseUnary() {
    if (match({TokenType::MINUS})) {
        auto operand = parseUnary();
        // Desugar -expr into (0 - expr)
        auto zero = std::make_unique<NumberLiteralNode>(0);
        return std::make_unique<BinaryOpNode>(
            std::move(zero), TokenType::MINUS, std::move(operand));
    }
    return parsePrimary();
}

// Primary: NUMBER | TRUE | FALSE | IDENTIFIER | '(' expression ')'
ASTNodePtr Parser::parsePrimary() {
    if (match({TokenType::NUMBER})) {
        int64_t val = std::stoll(previous().value);
        return std::make_unique<NumberLiteralNode>(val);
    }

    if (match({TokenType::TRUE})) {
        return std::make_unique<BoolLiteralNode>(true);
    }

    if (match({TokenType::FALSE})) {
        return std::make_unique<BoolLiteralNode>(false);
    }

    if (match({TokenType::IDENTIFIER})) {
        return std::make_unique<IdentifierNode>(previous().value);
    }

    if (match({TokenType::LPAREN})) {
        auto expr = parseExpression();
        consume(TokenType::RPAREN, "Expected ')' after expression");
        return expr;
    }

    error("Expected expression");
}
