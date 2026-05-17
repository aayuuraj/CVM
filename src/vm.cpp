#include "vm.h"
#include "bytecode.h"
#include <iostream>
#include <stdexcept>
#include <cstring>

VM::VM(const std::vector<uint8_t>& bytecode, size_t variableCount)
    : code_(bytecode), ip_(0) {
    // Pre-allocate variable slots initialized to integer 0
    variables_.resize(variableCount, Value::fromInt(0));
}

// ============================================================================
// Stack & read helpers
// ============================================================================

uint8_t VM::readByte() {
    return code_[ip_++];
}

int32_t VM::readInt32() {
    int32_t val;
    std::memcpy(&val, &code_[ip_], 4);
    ip_ += 4;
    return val;
}

int64_t VM::readInt64() {
    int64_t val;
    std::memcpy(&val, &code_[ip_], 8);
    ip_ += 8;
    return val;
}

void VM::push(Value val) {
    stack_.push_back(val);
}

Value VM::pop() {
    if (stack_.empty()) {
        throw std::runtime_error("VM error: stack underflow");
    }
    Value val = stack_.back();
    stack_.pop_back();
    return val;
}

const Value& VM::top() const {
    if (stack_.empty()) {
        throw std::runtime_error("VM error: stack underflow on top()");
    }
    return stack_.back();
}

// ============================================================================
// Main execution loop
// ============================================================================

void VM::run() {
    while (ip_ < code_.size()) {
        OpCode op = static_cast<OpCode>(readByte());

        switch (op) {
            case OpCode::PUSH_INT: {
                int64_t val = readInt64();
                push(Value::fromInt(val));
                break;
            }

            case OpCode::PUSH_BOOL: {
                uint8_t val = readByte();
                push(Value::fromBool(val != 0));
                break;
            }

            case OpCode::LOAD_VAR: {
                int32_t idx = readInt32();
                if (idx < 0 || static_cast<size_t>(idx) >= variables_.size()) {
                    throw std::runtime_error("VM error: variable index out of range");
                }
                push(variables_[idx]);
                break;
            }

            case OpCode::STORE_VAR: {
                int32_t idx = readInt32();
                if (idx < 0 || static_cast<size_t>(idx) >= variables_.size()) {
                    throw std::runtime_error("VM error: variable index out of range");
                }
                variables_[idx] = pop();
                break;
            }

            case OpCode::ADD: {
                Value b = pop();
                Value a = pop();
                push(Value::fromInt(a.asInt() + b.asInt()));
                break;
            }

            case OpCode::SUB: {
                Value b = pop();
                Value a = pop();
                push(Value::fromInt(a.asInt() - b.asInt()));
                break;
            }

            case OpCode::MUL: {
                Value b = pop();
                Value a = pop();
                push(Value::fromInt(a.asInt() * b.asInt()));
                break;
            }

            case OpCode::DIV: {
                Value b = pop();
                Value a = pop();
                if (b.asInt() == 0) {
                    throw std::runtime_error("VM error: division by zero");
                }
                push(Value::fromInt(a.asInt() / b.asInt()));
                break;
            }

            case OpCode::EQUAL: {
                Value b = pop();
                Value a = pop();
                // Both must be same type for equality
                if (a.type != b.type) {
                    push(Value::fromBool(false));
                } else if (a.type == Value::Type::INT) {
                    push(Value::fromBool(a.i == b.i));
                } else {
                    push(Value::fromBool(a.b == b.b));
                }
                break;
            }

            case OpCode::LESS_THAN: {
                Value b = pop();
                Value a = pop();
                push(Value::fromBool(a.asInt() < b.asInt()));
                break;
            }

            case OpCode::PRINT: {
                Value val = pop();
                val.print();
                std::cout << "\n";
                break;
            }

            case OpCode::INPUT: {
                int32_t idx = readInt32();
                if (idx < 0 || static_cast<size_t>(idx) >= variables_.size()) {
                    throw std::runtime_error("VM error: variable index out of range");
                }
                int64_t inputVal;
                std::cout << "? ";
                std::cout.flush();
                if (!(std::cin >> inputVal)) {
                    throw std::runtime_error("VM error: failed to read integer from stdin");
                }
                variables_[idx] = Value::fromInt(inputVal);
                break;
            }

            case OpCode::JUMP: {
                int32_t target = readInt32();
                ip_ = static_cast<size_t>(target);
                break;
            }

            case OpCode::JUMP_IF_FALSE: {
                int32_t target = readInt32();
                Value cond = pop();
                if (!cond.isTruthy()) {
                    ip_ = static_cast<size_t>(target);
                }
                break;
            }

            case OpCode::POP: {
                pop();
                break;
            }

            case OpCode::HALT: {
                return;
            }

            default:
                throw std::runtime_error(
                    "VM error: unknown opcode " + std::to_string(static_cast<int>(op)));
        }
    }
}
