#pragma once

#include "ast.h"
#include "bytecode.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>

// Bytecode compiler: walks the AST and emits a flat bytecode program
class Compiler {
public:
    Compiler();

    // Compile a ProgramNode into bytecode
    void compile(const ProgramNode& program);

    // Get the emitted bytecode
    const std::vector<uint8_t>& getBytecode() const;

    // Get the total number of variables declared
    size_t getVariableCount() const;

    // Disassemble the bytecode for debug output
    void disassemble() const;

private:
    std::vector<uint8_t> code_;                         // Emitted bytecode
    std::unordered_map<std::string, uint32_t> symbols_; // Variable name → index

    // --- Emit helpers ---

    // Emit a single opcode byte
    void emit(OpCode op);

    // Emit a raw byte
    void emitByte(uint8_t byte);

    // Emit a 32-bit signed integer (little-endian)
    void emitInt32(int32_t value);

    // Emit a 64-bit signed integer (little-endian)
    void emitInt64(int64_t value);

    // Get the current offset in the bytecode stream
    size_t currentOffset() const;

    // Patch a 32-bit value at a specific offset (for backpatching jumps)
    void patchInt32(size_t offset, int32_t value);

    // --- Variable management ---

    // Look up or create a variable index for the given name
    uint32_t resolveVariable(const std::string& name);

    // --- AST compilation ---

    // Compile a single AST node (dispatches by dynamic type)
    void compileNode(const ASTNode& node);

    // Compile an expression node (pushes a value onto the operand stack)
    void compileExpression(const ASTNode& node);
};
