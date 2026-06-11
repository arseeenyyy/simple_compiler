#pragma once

#include <string>
#include <vector> 
#include <exception>
#include <stdexcept>
#include "Token.h"

class LexerError : public std::runtime_error {
public: 
    int m_line;
    int m_column;

    LexerError(const std::string& message, int line, int column) 
        : std::runtime_error(message)
        , m_line(line) 
        , m_column(column) {}
};

class Lexer {
private:
    std::string m_source;
    size_t m_pos;
    int m_line;
    int m_col;

    // navigation through the source
    char peek() const;
    char peek_next() const;
    char advance();

    bool is_at_end() const;

    // reading separate lexems 
    Token read_number();
    Token read_string();
    Token read_identifier_or_keyword();
    Token read_operator_or_punct();

    void skip_whitespace();
    void skip_line_comment();

    void error(const std::string& message) const;

public:
    explicit Lexer(std::string source);
    
    std::vector<Token> tokenize();

};
