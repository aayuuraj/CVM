// ============================================================================
// CVM++ — Entry Point
// Supports: REPL mode, file execution, and --debug mode
// ============================================================================

#include "lexer.h"
#include "parser.h"
#include "compiler.h"
#include "vm.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// Read an entire file into a string
static std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + path);
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

// Run CVM source code through the full pipeline: lex → parse → compile → execute
static void runSource(const std::string& source, bool debug) {
    // 1. Lexing
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    if (debug) {
        std::cout << "=== TOKENS ===\n";
        for (auto& tok : tokens) {
            std::cout << tokenToString(tok) << " ";
        }
        std::cout << "\n\n";
    }

    // 2. Parsing
    Parser parser(tokens);
    auto ast = parser.parse();

    if (debug) {
        std::cout << "=== AST ===\n";
        ast->dump(0);
        std::cout << "\n";
    }

    // 3. Compilation
    Compiler compiler;
    compiler.compile(*ast);

    if (debug) {
        std::cout << "=== BYTECODE ===\n";
        compiler.disassemble();
        std::cout << "\n";
        std::cout << "=== EXECUTION OUTPUT ===\n";
    }

    // 4. Execution
    VM vm(compiler.getBytecode(), compiler.getVariableCount());
    vm.run();
}

// Interactive REPL mode
static void runREPL() {
    std::cout << "CVM++ REPL v1.0  (type 'exit' to quit)\n";
    std::string line;
    while (true) {
        std::cout << ">>> ";
        std::cout.flush();
        if (!std::getline(std::cin, line)) break;
        if (line == "exit" || line == "quit") break;
        if (line.empty()) continue;

        try {
            runSource(line, false);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }
}

// Print usage information
static void printUsage(const char* progName) {
    std::cout << "Usage:\n"
              << "  " << progName << "                 Start the REPL\n"
              << "  " << progName << " <file.cvm>      Run a CVM source file\n"
              << "  " << progName << " --debug <file>   Run with debug output\n";
}

int main(int argc, char* argv[]) {
    try {
        if (argc == 1) {
            // No arguments: start REPL
            runREPL();
        } else if (argc == 2) {
            std::string arg1 = argv[1];
            if (arg1 == "--help" || arg1 == "-h") {
                printUsage(argv[0]);
                return 0;
            }
            // Single argument: run file
            std::string source = readFile(arg1);
            runSource(source, false);
        } else if (argc == 3) {
            std::string arg1 = argv[1];
            std::string arg2 = argv[2];
            if (arg1 == "--debug") {
                // Debug mode: print tokens, AST, bytecode, then run
                std::string source = readFile(arg2);
                runSource(source, true);
            } else {
                printUsage(argv[0]);
                return 1;
            }
        } else {
            printUsage(argv[0]);
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
