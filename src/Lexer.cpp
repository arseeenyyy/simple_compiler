#include <cctype>
#include <stdexcept>
#include <sstream>
#include "Lexer.h"

const std::unordered_map<std::string, TokenType> KEYWORDS = {
    {"var", TokenType::KW_VAR},
    {"print", TokenType::KW_PRINT},
    {"if", TokenType::KW_IF},
    {"else", TokenType::KW_ELSE},
    {"while", TokenType::KW_WHILE},
    {"true", TokenType::KW_TRUE},
    {"false", TokenType::KW_FALSE},
    {"fun", TokenType::KW_FUN},
    {"return", TokenType::KW_RETURN}
};

const std::unordered_map<std::string, TokenType> OPERATORS = {
    {"==", TokenType::EQEQ},
    {"!=", TokenType::NEQ},
    {"<=", TokenType::LTEQ},
    {">=", TokenType::GTEQ},
    {"&&", TokenType::AND},
    {"||", TokenType::OR},
    {"+", TokenType::PLUS},
    {"-", TokenType::MINUS},
    {"*", TokenType::STAR},
    {"/", TokenType::SLASH},
    {"=", TokenType::EQ},
    {"<", TokenType::LT},
    {">", TokenType::GT},
    {"!", TokenType::EXCL},
    {"(", TokenType::LPAREN},
    {")", TokenType::RPAREN},
    {"{", TokenType::LBRACE},
    {"}", TokenType::RBRACE},
    {"[", TokenType::LBRACKET},
    {"]", TokenType::RBRACKET},
    {";", TokenType::SEMICOLON},
    {",", TokenType::COMMA}
};

void Lexer::error(const std::string& message) const {
    std::ostringstream oss;
    oss << "[" << m_line << ":" << m_col << "] Lexer error: " << message;
    throw LexerError(oss.str(), m_line, m_col);
}


Lexer::Lexer(std::string source)
    : m_source(std::move(source)), m_pos(0), m_line(1), m_col(1) {}
std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (!is_at_end()) {
        skip_whitespace();
        if (is_at_end()) break;

        if (peek() == '/' && peek_next() == '/') {
            skip_line_comment();
            continue;
        }

        char c = peek();

        if (std::isdigit(static_cast<unsigned char>(c))) {
            tokens.push_back(read_number());
        } else if (c == '"') {
            tokens.push_back(read_string());
        } else if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            tokens.push_back(read_identifier_or_keyword());
        } else {
            tokens.push_back(read_operator_or_punct());
        }
    }

    tokens.emplace_back(TokenType::END_OF_FILE, "", m_line, m_col);
    return tokens;
}


char Lexer::peek() const {
    return is_at_end() ? '\0' : m_source[m_pos];
}

char Lexer::peek_next() const {
    return (m_pos + 1 < m_source.size()) ? m_source[m_pos + 1] : '\0';
}

char Lexer::advance() {
    char c = m_source[m_pos ++];
    if (c == '\n') {
        ++m_line;
        m_col = 1;
    } else {
        ++ m_col; 
    }
    return c;
}

bool Lexer::is_at_end() const {
    return m_pos >= m_source.size();
}

void Lexer::skip_whitespace() {
    while (!is_at_end() && std::isspace(static_cast<unsigned char>(peek()))) {
        advance();
    }
}

void Lexer::skip_line_comment() {
    while (!is_at_end() && peek() != '\n') {
        advance();
    }
}

Token Lexer::read_number() {
    int start_line = m_line;
    int start_col = m_col;
    std::string value;
    while (!is_at_end() && std::isdigit(static_cast<unsigned char>(peek()))) {
        value += advance();
    }

    if (!is_at_end() && peek() == '.' && std::isdigit(static_cast<unsigned char>(peek_next()))) {
        value += advance(); 
        while (!is_at_end() && std::isdigit(static_cast<unsigned char>(peek()))) {
            value += advance();
        }
    }
    return Token(TokenType::NUMBER, value, start_line, start_col);
}

Token Lexer::read_string() {
    int start_line = m_line;
    int start_col  = m_col;
    advance(); 

    std::string value;
    while (!is_at_end() && peek() != '"') {
        if (peek() == '\n') {
            error("Unclosed string literal");
        }
        if (peek() == '\\') {
            advance();
            switch (peek()) {
                case 'n':  value += '\n'; advance(); break;
                case 't':  value += '\t'; advance(); break;
                case '"':  value += '"';  advance(); break;
                case '\\': value += '\\'; advance(); break;
                default:
                    error("Unknown escape sequence");
            }
        } else {
            value += advance();
        }
    }

    if (is_at_end()) {
        error("Unclosed string literal");
    }

    advance(); 
    return Token(TokenType::STRING, value, start_line, start_col);
}

Token Lexer::read_identifier_or_keyword() {
    int start_line = m_line;
    int start_col  = m_col;
    std::string value;

    while (!is_at_end() &&
           (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_')) {
        value += advance();
    }

    auto it = KEYWORDS.find(value);
    TokenType type = (it != KEYWORDS.end()) ? it->second : TokenType::IDENTIFIER;

    return Token(type, value, start_line, start_col);
}

Token Lexer::read_operator_or_punct() {
    int start_line = m_line;
    int start_col = m_col;
    
    if (!is_at_end() && peek_next()) {
        std::string twoChars = {peek(), peek_next()};
        auto it = OPERATORS.find(twoChars);
        if (it != OPERATORS.end()) {
            advance();
            advance();
            return Token(it->second, twoChars, start_line, start_col);
        }
    }
    
    std::string oneChar = {peek()};
    auto it = OPERATORS.find(oneChar);
    if (it != OPERATORS.end()) {
        advance();
        return Token(it->second, oneChar, start_line, start_col);
    }
    
    error("Unexpected character: " + std::string(1, peek()));
    advance();
    return Token(TokenType::UNKNOWN, "", start_line, start_col);
}