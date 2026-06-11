cmake_minimum_required(VERSION 3.14)
set(CMAKE_BUILD_TYPE Debug)
project(Compiler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)

include_directories(include)

set(SOURCES
    src/main.cpp
    src/Lexer.cpp
    src/Token.cpp
    src/TokenType.cpp
)

add_executable(compiler ${SOURCES})
