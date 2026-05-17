#include "compiler.h"
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <cstring>

Compiler::Compiler() {}

// ============================================================================
// Public interface
// ============================================================================

void Compiler::compile(const ProgramNode& program) {
    for (auto& stmt : program.statements) {
        compileNode(*stmt);
    }
    emit(OpCode::HALT);
}

const std::vector<uint8_t>& Compiler::getBytecode() const {
    return code_;
}

size_t Compiler::getVariableCount() const {
    return symbols_.size();
}

// ============================================================================
// Emit helpers
// ============================================================================

void Compiler::emit(OpCode op) {
    code_.push_back(static_cast<uint8_t>(op));
}

void Compiler::emitByte(uint8_t byte) {
    code_.push_back(byte);
}

void Compiler::emitInt32(int32_t value) {
    // Little-endian encoding
    uint8_t bytes[4];
    std::memcpy(bytes, &value, 4);
    for (int i = 0; i < 4; i++) {
        code_.push_back(bytes[i]);
    }
}

void Compiler::emitInt64(int64_t value) {
    // Little-endian encoding
    uint8_t bytes[8];
    std::memcpy(bytes, &value, 8);
    for (int i = 0; i < 8; i++) {
        code_.push_back(bytes[i]);
    }
}

size_t Compiler::currentOffset() const {
    return code_.size();
}

void Compiler::patchInt32(size_t offset, int32_t value) {
    std::memcpy(&code_[offset], &value, 4);
}

// ============================================================================
// Variable management
// ============================================================================

uint32_t Compiler::resolveVariable(const std::string& name) {
    auto it = symbols_.find(name);
    if (it != symbols_.end()) {
        return it->second;
    }
    uint32_t index = static_cast<uint32_t>(symbols_.size());
    symbols_[name] = index;
    return index;
}

// ============================================================================
// AST compilation: dispatching by node type
// ============================================================================

void Compiler::compileNode(const ASTNode& node) {
    // --- Statements ---

    if (auto* let = dynamic_cast<const LetNode*>(&node)) {
        // Compile initializer → pushes value onto stack
        compileExpression(*let->initializer);
        uint32_t idx = resolveVariable(let->name);
        emit(OpCode::STORE_VAR);
        emitInt32(static_cast<int32_t>(idx));
        return;
    }

    if (auto* assign = dynamic_cast<const AssignNode*>(&node)) {
        compileExpression(*assign->value);
        auto it = symbols_.find(assign->name);
        if (it == symbols_.end()) {
            throw std::runtime_error("Compile error: undefined variable '" + assign->name + "'");
        }
        emit(OpCode::STORE_VAR);
        emitInt32(static_cast<int32_t>(it->second));
        return;
    }

    if (auto* print = dynamic_cast<const PrintNode*>(&node)) {
        compileExpression(*print->expression);
        emit(OpCode::PRINT);
        return;
    }

    if (auto* input = dynamic_cast<const InputNode*>(&node)) {
        uint32_t idx = resolveVariable(input->variableName);
        emit(OpCode::INPUT);
        emitInt32(static_cast<int32_t>(idx));
        return;
    }

    if (auto* block = dynamic_cast<const BlockNode*>(&node)) {
        for (auto& stmt : block->statements) {
            compileNode(*stmt);
        }
        return;
    }

    if (auto* ifNode = dynamic_cast<const IfNode*>(&node)) {
        // Compile condition
        compileExpression(*ifNode->condition);

        // JUMP_IF_FALSE → jump past then-block
        emit(OpCode::JUMP_IF_FALSE);
        size_t jumpToElse = currentOffset();
        emitInt32(0);  // placeholder

        // Compile then-block
        compileNode(*ifNode->thenBlock);

        if (ifNode->elseBlock) {
            // JUMP → jump past else-block (at end of then-block)
            emit(OpCode::JUMP);
            size_t jumpPastElse = currentOffset();
            emitInt32(0);  // placeholder

            // Patch JUMP_IF_FALSE to land here (start of else-block)
            patchInt32(jumpToElse, static_cast<int32_t>(currentOffset()));

            // Compile else-block
            compileNode(*ifNode->elseBlock);

            // Patch JUMP to land here (after else-block)
            patchInt32(jumpPastElse, static_cast<int32_t>(currentOffset()));
        } else {
            // No else: patch JUMP_IF_FALSE to land after then-block
            patchInt32(jumpToElse, static_cast<int32_t>(currentOffset()));
        }
        return;
    }

    if (auto* whileNode = dynamic_cast<const WhileNode*>(&node)) {
        // Record start of loop (where condition is evaluated)
        size_t loopStart = currentOffset();

        // Compile condition
        compileExpression(*whileNode->condition);

        // JUMP_IF_FALSE → jump past loop body
        emit(OpCode::JUMP_IF_FALSE);
        size_t jumpExit = currentOffset();
        emitInt32(0);  // placeholder

        // Compile loop body
        compileNode(*whileNode->body);

        // JUMP back to loop start
        emit(OpCode::JUMP);
        emitInt32(static_cast<int32_t>(loopStart));

        // Patch JUMP_IF_FALSE to land here (after loop)
        patchInt32(jumpExit, static_cast<int32_t>(currentOffset()));
        return;
    }

    if (auto* prog = dynamic_cast<const ProgramNode*>(&node)) {
        for (auto& stmt : prog->statements) {
            compileNode(*stmt);
        }
        return;
    }

    // If we get here, it's an expression statement — compile and pop the result
    compileExpression(node);
    emit(OpCode::POP);
}

void Compiler::compileExpression(const ASTNode& node) {
    if (auto* num = dynamic_cast<const NumberLiteralNode*>(&node)) {
        emit(OpCode::PUSH_INT);
        emitInt64(num->value);
        return;
    }

    if (auto* boolLit = dynamic_cast<const BoolLiteralNode*>(&node)) {
        emit(OpCode::PUSH_BOOL);
        emitByte(boolLit->value ? 1 : 0);
        return;
    }

    if (auto* ident = dynamic_cast<const IdentifierNode*>(&node)) {
        auto it = symbols_.find(ident->name);
        if (it == symbols_.end()) {
            throw std::runtime_error("Compile error: undefined variable '" + ident->name + "'");
        }
        emit(OpCode::LOAD_VAR);
        emitInt32(static_cast<int32_t>(it->second));
        return;
    }

    if (auto* binop = dynamic_cast<const BinaryOpNode*>(&node)) {
        // Compile left, then right, then operator
        compileExpression(*binop->left);
        compileExpression(*binop->right);
        switch (binop->op) {
            case TokenType::PLUS:        emit(OpCode::ADD); break;
            case TokenType::MINUS:       emit(OpCode::SUB); break;
            case TokenType::STAR:        emit(OpCode::MUL); break;
            case TokenType::SLASH:       emit(OpCode::DIV); break;
            case TokenType::EQUAL_EQUAL: emit(OpCode::EQUAL); break;
            case TokenType::LESS:        emit(OpCode::LESS_THAN); break;
            default:
                throw std::runtime_error("Compile error: unknown binary operator");
        }
        return;
    }

    throw std::runtime_error("Compile error: cannot compile expression node");
}

// ============================================================================
// Disassembler
// ============================================================================

void Compiler::disassemble() const {
    size_t offset = 0;
    while (offset < code_.size()) {
        std::cout << std::setw(4) << std::setfill('0') << offset << ": ";

        OpCode op = static_cast<OpCode>(code_[offset++]);
        std::cout << opcodeName(op);

        switch (op) {
            case OpCode::PUSH_INT: {
                int64_t val;
                std::memcpy(&val, &code_[offset], 8);
                offset += 8;
                std::cout << " " << val;
                break;
            }
            case OpCode::PUSH_BOOL: {
                uint8_t val = code_[offset++];
                std::cout << " " << (val ? "true" : "false");
                break;
            }
            case OpCode::LOAD_VAR:
            case OpCode::STORE_VAR:
            case OpCode::INPUT: {
                int32_t idx;
                std::memcpy(&idx, &code_[offset], 4);
                offset += 4;
                std::cout << " [" << idx << "]";
                break;
            }
            case OpCode::JUMP:
            case OpCode::JUMP_IF_FALSE: {
                int32_t target;
                std::memcpy(&target, &code_[offset], 4);
                offset += 4;
                std::cout << " -> " << target;
                break;
            }
            default:
                break;
        }

        std::cout << "\n";
    }
}
