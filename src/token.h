#pragma once

#include <string>
#include <unordered_map>

// All token types recognized by the CVM lexer
enum class TokenType {
    // Literals
    NUMBER,
    IDENTIFIER,
    TRUE,
    FALSE,

    // Operators
    PLUS,
    MINUS,
    STAR,
    SLASH,
    EQUAL,          // Assignment =
    EQUAL_EQUAL,    // Comparison ==
    LESS,           // Comparison <

    // Keywords
    LET,
    IF,
    ELSE,
    WHILE,
    PRINT,
    INPUT,

    // Delimiters
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    SEMICOLON,

    // Special
    EOF_TOKEN
};

// A single token produced by the lexer
struct Token {
    TokenType type;
    std::string value;  // Lexeme text
    int line;           // Source line number (1-based)
    int col;            // Source column number (1-based)

    Token(TokenType type, std::string value, int line, int col)
        : type(type), value(std::move(value)), line(line), col(col) {}
};

// Convert a TokenType to its human-readable name
inline std::string tokenTypeName(TokenType t) {
    switch (t) {
        case TokenType::NUMBER:       return "NUMBER";
        case TokenType::IDENTIFIER:   return "IDENTIFIER";
        case TokenType::TRUE:         return "TRUE";
        case TokenType::FALSE:        return "FALSE";
        case TokenType::PLUS:         return "PLUS";
        case TokenType::MINUS:        return "MINUS";
        case TokenType::STAR:         return "STAR";
        case TokenType::SLASH:        return "SLASH";
        case TokenType::EQUAL:        return "EQUAL";
        case TokenType::EQUAL_EQUAL:  return "EQUAL_EQUAL";
        case TokenType::LESS:         return "LESS";
        case TokenType::LET:         return "LET";
        case TokenType::IF:          return "IF";
        case TokenType::ELSE:        return "ELSE";
        case TokenType::WHILE:       return "WHILE";
        case TokenType::PRINT:       return "PRINT";
        case TokenType::INPUT:       return "INPUT";
        case TokenType::LPAREN:      return "LPAREN";
        case TokenType::RPAREN:      return "RPAREN";
        case TokenType::LBRACE:      return "LBRACE";
        case TokenType::RBRACE:      return "RBRACE";
        case TokenType::SEMICOLON:   return "SEMICOLON";
        case TokenType::EOF_TOKEN:   return "EOF";
    }
    return "UNKNOWN";
}

// Convert a token to a debug string like "NUMBER(42)"
inline std::string tokenToString(const Token& t) {
    switch (t.type) {
        case TokenType::NUMBER:
        case TokenType::IDENTIFIER:
            return tokenTypeName(t.type) + "(" + t.value + ")";
        default:
            return tokenTypeName(t.type);
    }
}
