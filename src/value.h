#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <stdexcept>

// Runtime value type used on the VM operand stack and in variables
struct Value {
    enum class Type { INT, BOOL } type;

    union {
        int64_t i;
        bool b;
    };

    // Create an integer value
    static Value fromInt(int64_t v) {
        Value val;
        val.type = Type::INT;
        val.i = v;
        return val;
    }

    // Create a boolean value
    static Value fromBool(bool v) {
        Value val;
        val.type = Type::BOOL;
        val.b = v;
        return val;
    }

    // Print the value to stdout
    void print() const {
        switch (type) {
            case Type::INT:  std::cout << i; break;
            case Type::BOOL: std::cout << (b ? "true" : "false"); break;
        }
    }

    // Convert to a display string
    std::string toString() const {
        switch (type) {
            case Type::INT:  return std::to_string(i);
            case Type::BOOL: return b ? "true" : "false";
        }
        return "?";
    }

    // Extract the integer value, or throw on type mismatch
    int64_t asInt() const {
        if (type != Type::INT)
            throw std::runtime_error("Expected integer value");
        return i;
    }

    // Extract the boolean value, or throw on type mismatch
    bool asBool() const {
        if (type != Type::BOOL)
            throw std::runtime_error("Expected boolean value");
        return b;
    }

    // Truthiness: integers are truthy if non-zero, bools by their value
    bool isTruthy() const {
        switch (type) {
            case Type::INT:  return i != 0;
            case Type::BOOL: return b;
        }
        return false;
    }
};
