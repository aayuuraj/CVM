cmake_minimum_required(VERSION 3.16)
project(CVM++ VERSION 1.0 LANGUAGES CXX)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings
add_compile_options(-Wall -Wextra -Wpedantic)

# Collect all source files
set(SOURCES
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/compiler.cpp
    src/vm.cpp
)

# Build the cvm executable
add_executable(cvm ${SOURCES})

# Include the src/ directory for header resolution
target_include_directories(cvm PRIVATE ${CMAKE_SOURCE_DIR}/src)
