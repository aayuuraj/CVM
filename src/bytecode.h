#pragma once

#include <cstdint>
#include <string>

// All opcodes for the CVM bytecode instruction set
enum class OpCode : uint8_t {
    PUSH_INT,       // Push a 64-bit integer constant (followed by 8 bytes)
    PUSH_BOOL,      // Push a boolean value (followed by 1 byte: 0 or 1)
    LOAD_VAR,       // Load variable by index (followed by 4-byte uint32_t index)
    STORE_VAR,      // Store top-of-stack into variable by index (followed by 4-byte uint32_t index)
    ADD,            // Pop two values, push their sum
    SUB,            // Pop two values, push their difference (second - first)
    MUL,            // Pop two values, push their product
    DIV,            // Pop two values, push their quotient (second / first)
    EQUAL,          // Pop two values, push bool (second == first)
    LESS_THAN,      // Pop two values, push bool (second < first)
    PRINT,          // Pop and print the top value
    INPUT,          // Read integer from stdin, store in variable (followed by 4-byte uint32_t index)
    JUMP,           // Unconditional jump (followed by 4-byte int32_t offset)
    JUMP_IF_FALSE,  // Pop top; if falsy, jump (followed by 4-byte int32_t offset)
    POP,            // Discard the top value
    HALT            // Stop execution
};

// Convert an opcode to its human-readable name
inline std::string opcodeName(OpCode op) {
    switch (op) {
        case OpCode::PUSH_INT:      return "PUSH_INT";
        case OpCode::PUSH_BOOL:     return "PUSH_BOOL";
        case OpCode::LOAD_VAR:      return "LOAD_VAR";
        case OpCode::STORE_VAR:     return "STORE_VAR";
        case OpCode::ADD:           return "ADD";
        case OpCode::SUB:           return "SUB";
        case OpCode::MUL:           return "MUL";
        case OpCode::DIV:           return "DIV";
        case OpCode::EQUAL:         return "EQUAL";
        case OpCode::LESS_THAN:     return "LESS_THAN";
        case OpCode::PRINT:         return "PRINT";
        case OpCode::INPUT:         return "INPUT";
        case OpCode::JUMP:          return "JUMP";
        case OpCode::JUMP_IF_FALSE: return "JUMP_IF_FALSE";
        case OpCode::POP:           return "POP";
        case OpCode::HALT:          return "HALT";
    }
    return "UNKNOWN";
}
