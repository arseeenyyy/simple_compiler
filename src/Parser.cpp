#include "Parser.h"

ParseError::ParseError(const std::string& message) 
    : m_message(message) {} 

const char* ParseError::what() const noexcept {
    return m_message.c_str();
}

Parser::Parser(std::vector<Token> tokens) 
    : m_tokens(std::move(tokens))
    , m_position(0) {} 

std::vector<std::unique_ptr<Statement>> Parser::parse() {
    std::vector<std::unique_ptr<Statement>> statements;

    while (is_at_end()) {
        statements.push_back(parse_declaration());
    }
    return statements;
}

std::unique_ptr<Statement> Parser::parse_declaration() {
    return nullptr;
}



bool Parser::is_at_end() const { 
    return peek().get_type() == TokenType::END_OF_FILE;
}

Token Parser::peek() const {
    return m_tokens[m_position];
}

Token Parser::previous() const {
    return m_tokens[m_position - 1];
}

Token Parser::advance() {
    if (!is_at_end()) {
        ++m_position;
    } 
    return previous();
}

bool Parser::check(TokenType type) const {
    if (is_at_end()) return false;
    return peek().get_type() == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::match(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    error(peek(), message);
    throw ParseError(message);
}

void Parser::error(const Token& token, const std::string& message) {
    throw ParseError("[" + std::to_string(token.get_line()) + ":" + 
                     std::to_string(token.get_column()) + "] Parser error: " + message);
}

void Parser::synchronize() {
    advance();
    
    while (!is_at_end()) {
        if (previous().get_type() == TokenType::SEMICOLON) return;
        
        switch (peek().get_type()) {
            case TokenType::KW_VAR:
            case TokenType::KW_PRINT:
            case TokenType::KW_IF:
            case TokenType::KW_WHILE:
            case TokenType::KW_FUN:
            case TokenType::KW_RETURN:
                return;
            default:
                advance();
        }
    }
}