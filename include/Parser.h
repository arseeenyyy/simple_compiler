#pragma once

#include <vector>
#include <memory>
#include "Token.h"
#include "AST.h"

class ParseError : public std::exception {
public:
    explicit ParseError(const std::string& message);
    const char* what() const noexcept override;

private:
    std::string m_message;
};

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    
    std::vector<std::unique_ptr<Statement>> parse();

private:
    std::vector<Token> m_tokens;
    size_t m_position = 0;

    std::unique_ptr<Statement> parse_declaration();
    std::unique_ptr<Statement> parse_statement();
    std::unique_ptr<Statement> parse_var_declaration();
    std::unique_ptr<Statement> parse_print_statement();
    std::unique_ptr<Statement> parse_if_statement();
    std::unique_ptr<Statement> parse_while_statement();
    std::unique_ptr<Statement> parse_block_statement();
    std::unique_ptr<Statement> parse_expression_statement();
    std::unique_ptr<Statement> parse_function_declaration();
    std::unique_ptr<Statement> parse_return_statement();
    
    std::unique_ptr<Expression> parse_expression();
    std::unique_ptr<Expression> parse_assignment();
    std::unique_ptr<Expression> parse_logical_or();
    std::unique_ptr<Expression> parse_logical_and();
    std::unique_ptr<Expression> parse_equality();
    std::unique_ptr<Expression> parse_comparison();
    std::unique_ptr<Expression> parse_term();
    std::unique_ptr<Expression> parse_factor();
    std::unique_ptr<Expression> parse_unary();
    std::unique_ptr<Expression> parse_primary();
    
    std::unique_ptr<Expression> parse_array_literal();
    std::unique_ptr<Expression> parse_index_expression(std::unique_ptr<Expression> array);
    std::unique_ptr<Expression> parse_call_expression(const Token& name);
    std::vector<std::string> parse_parameters();
    std::vector<std::unique_ptr<Expression>> parse_arguments();
    std::vector<std::unique_ptr<Statement>> parse_block();
    
    bool match(TokenType type);
    bool match(std::initializer_list<TokenType> types);
    bool check(TokenType type) const;
    Token advance();
    Token peek() const;
    Token previous() const;
    bool is_at_end() const;
    Token consume(TokenType type, const std::string& message);
    void synchronize();
    
    void error(const Token& token, const std::string& message);
};