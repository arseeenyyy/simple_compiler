#pragma once

#include <string> 
#include "TokenType.h"



class Token {
private:
    TokenType m_type = TokenType::UNKNOWN;
    std::string m_value;
    int m_line = 0;
    int m_column = 0;

public:

    Token() = default;
    Token(TokenType type, std::string value, int line, int column) 
        : m_type(type) 
        , m_value(std::move(value)) 
        , m_line(line) 
        , m_column(column) {}

    TokenType get_type() const noexcept { return m_type; } 
    const std::string& get_value() const noexcept { return m_value; }
    int get_line() const noexcept { return m_line; }
    int get_column() const noexcept { return m_column; }


    std::string to_string() const;

    bool operator==(const Token& other) const noexcept {
        return m_type == other.m_type && m_value == other.m_value;
    }
    
};
