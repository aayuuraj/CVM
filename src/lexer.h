#pragma once

#include "token.h"
#include <string>
#include <vector>

// Lexer: converts raw CVM source code into a sequence of tokens
class Lexer {
public:
    // Construct a lexer for the given source string
    explicit Lexer(const std::string& source);

    // Tokenize the entire source and return all tokens (ending with EOF_TOKEN)
    std::vector<Token> tokenize();

private:
    std::string source_;
    size_t pos_;
    int line_;
    int col_;

    // Peek at the current character without advancing
    char peek() const;

    // Peek at the next character (one ahead of current)
    char peekNext() const;

    // Advance the position and return the current character
    char advance();

    // Check if we've reached the end of source
    bool isAtEnd() const;

    // Skip whitespace and newline characters
    void skipWhitespace();

    // Skip single-line comments (// ...)
    void skipLineComment();

    // Read a number literal token
    Token readNumber();

    // Read an identifier or keyword token
    Token readIdentifier();

    // Read the next token from the source
    Token nextToken();
};
