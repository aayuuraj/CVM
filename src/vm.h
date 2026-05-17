#pragma once

#include "value.h"
#include <vector>
#include <cstdint>

// Stack-based virtual machine: executes CVM bytecode
class VM {
public:
    // Construct a VM with the given bytecode and variable count
    VM(const std::vector<uint8_t>& bytecode, size_t variableCount);

    // Execute the bytecode program
    void run();

private:
    std::vector<uint8_t> code_;       // The bytecode program
    std::vector<Value> stack_;        // Operand stack
    std::vector<Value> variables_;    // Variable storage (indexed)
    size_t ip_;                       // Instruction pointer

    // --- Helper methods ---

    // Read the next byte and advance IP
    uint8_t readByte();

    // Read a 32-bit signed integer and advance IP
    int32_t readInt32();

    // Read a 64-bit signed integer and advance IP
    int64_t readInt64();

    // Push a value onto the operand stack
    void push(Value val);

    // Pop a value from the operand stack
    Value pop();

    // Peek at the top of the stack without popping
    const Value& top() const;
};
