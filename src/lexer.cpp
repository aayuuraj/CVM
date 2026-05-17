#include "lexer.h"
#include <stdexcept>
#include <unordered_map>

// Keyword lookup table
static const std::unordered_map<std::string, TokenType> keywords = {
    {"let",   TokenType::LET},
    {"if",    TokenType::IF},
    {"else",  TokenType::ELSE},
    {"while", TokenType::WHILE},
    {"print", TokenType::PRINT},
    {"input", TokenType::INPUT},
    {"true",  TokenType::TRUE},
    {"false", TokenType::FALSE},
};

Lexer::Lexer(const std::string& source)
    : source_(source), pos_(0), line_(1), col_(1) {}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source_[pos_];
}

char Lexer::peekNext() const {
    if (pos_ + 1 >= source_.size()) return '\0';
    return source_[pos_ + 1];
}

char Lexer::advance() {
    char c = source_[pos_++];
    if (c == '\n') {
        line_++;
        col_ = 1;
    } else {
        col_++;
    }
    return c;
}

bool Lexer::isAtEnd() const {
    return pos_ >= source_.size();
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            advance();
        } else if (c == '/' && peekNext() == '/') {
            skipLineComment();
        } else {
            break;
        }
    }
}

void Lexer::skipLineComment() {
    // Skip the '//' characters
    advance();
    advance();
    // Skip until end of line
    while (!isAtEnd() && peek() != '\n') {
        advance();
    }
}

Token Lexer::readNumber() {
    int startCol = col_;
    std::string num;
    while (!isAtEnd() && std::isdigit(peek())) {
        num += advance();
    }
    return Token(TokenType::NUMBER, num, line_, startCol);
}

Token Lexer::readIdentifier() {
    int startCol = col_;
    std::string ident;
    while (!isAtEnd() && (std::isalnum(peek()) || peek() == '_')) {
        ident += advance();
    }
    // Check if the identifier is a keyword
    auto it = keywords.find(ident);
    if (it != keywords.end()) {
        return Token(it->second, ident, line_, startCol);
    }
    return Token(TokenType::IDENTIFIER, ident, line_, startCol);
}

Token Lexer::nextToken() {
    skipWhitespace();

    if (isAtEnd()) {
        return Token(TokenType::EOF_TOKEN, "", line_, col_);
    }

    int startLine = line_;
    int startCol = col_;
    char c = peek();

    // Number literals
    if (std::isdigit(c)) {
        return readNumber();
    }

    // Identifiers and keywords
    if (std::isalpha(c) || c == '_') {
        return readIdentifier();
    }

    // Single and multi-character tokens
    advance();
    switch (c) {
        case '+': return Token(TokenType::PLUS,      "+", startLine, startCol);
        case '-': return Token(TokenType::MINUS,     "-", startLine, startCol);
        case '*': return Token(TokenType::STAR,      "*", startLine, startCol);
        case '/': return Token(TokenType::SLASH,     "/", startLine, startCol);
        case '(': return Token(TokenType::LPAREN,    "(", startLine, startCol);
        case ')': return Token(TokenType::RPAREN,    ")", startLine, startCol);
        case '{': return Token(TokenType::LBRACE,    "{", startLine, startCol);
        case '}': return Token(TokenType::RBRACE,    "}", startLine, startCol);
        case ';': return Token(TokenType::SEMICOLON, ";", startLine, startCol);
        case '<': return Token(TokenType::LESS,      "<", startLine, startCol);
        case '=':
            if (!isAtEnd() && peek() == '=') {
                advance();
                return Token(TokenType::EQUAL_EQUAL, "==", startLine, startCol);
            }
            return Token(TokenType::EQUAL, "=", startLine, startCol);
        default:
            throw std::runtime_error(
                "Lexer error at line " + std::to_string(startLine) +
                ", col " + std::to_string(startCol) +
                ": unrecognized character '" + std::string(1, c) + "'");
    }
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (true) {
        Token tok = nextToken();
        tokens.push_back(tok);
        if (tok.type == TokenType::EOF_TOKEN) break;
    }
    return tokens;
}
