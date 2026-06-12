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
    if (match(TokenType::KW_FUN)) {
        return parse_function_declaration();
    } 
    if (match(TokenType::KW_VAR)) {
        return parse_var_declaration();
    }
    return parse_statement();
}

std::unique_ptr<Statement> Parser::parse_statement() {
    if (match(TokenType::KW_RETURN)) {
        return parse_return_statement();
    }
    if (match(TokenType::KW_IF)) {
        return parse_if_statement();
    }
    if (match(TokenType::KW_WHILE)) {
        return parse_while_statement();
    }
    if (match(TokenType::KW_PRINT)) {
        return parse_print_statement();
    }
    if (match(TokenType::LBRACE)) {
        return parse_block_statement();
    }
    return parse_expression_statement();
}

std::unique_ptr<Statement> Parser::parse_var_declaration() {
    Token keyword = previous();
    Token name = consume(TokenType::IDENTIFIER, "Expected variable name");
    
    std::unique_ptr<Expression> initializer = nullptr;
    if (match(TokenType::EQ)) {
        initializer = parse_expression();
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
    
    return std::make_unique<VarStatement>(
        name.get_value(), std::move(initializer), keyword.get_line(), keyword.get_column()
    );
}

std::unique_ptr<Statement> Parser::parse_print_statement() {
    Token keyword = previous();
    auto value = parse_expression();
    consume(TokenType::SEMICOLON, "Expected ';' after value");
    
    return std::make_unique<PrintStatement>(std::move(value), keyword.get_line(), keyword.get_column());
}

std::unique_ptr<Statement> Parser::parse_if_statement() {
    Token keyword = previous();
    consume(TokenType::LPAREN, "Expected '(' after 'if'");
    auto condition = parse_expression();
    consume(TokenType::RPAREN, "Expected ')' after condition");
    
    auto then_branch = parse_statement();
    std::unique_ptr<Statement> else_branch = nullptr;
    
    if (match(TokenType::KW_ELSE)) {
        else_branch = parse_statement();
    }
    
    return std::make_unique<IfStatement>(
        std::move(condition), std::move(then_branch), std::move(else_branch), keyword.get_line(), keyword.get_column()
    );
}

std::unique_ptr<Statement> Parser::parse_while_statement() {
    Token keyword = previous();
    consume(TokenType::LPAREN, "Expected '(' after 'while'");
    auto condition = parse_expression();
    consume(TokenType::RPAREN, "Expected ')' after condition");
    
    auto body = parse_statement();
    
    return std::make_unique<WhileStatement>(
        std::move(condition), std::move(body), keyword.get_line(), keyword.get_column()
    );
}

std::unique_ptr<Statement> Parser::parse_block_statement() {
    int line = previous().get_line();
    int column = previous().get_column();
    auto statements = parse_block();
    consume(TokenType::RBRACE, "Expected '}' after block");
    
    return std::make_unique<BlockStatement>(std::move(statements), line, column);
}
std::unique_ptr<Statement> Parser::parse_expression_statement() {
    auto expr = parse_expression();
    consume(TokenType::SEMICOLON, "Expected ';' after expression");
    
    return std::make_unique<ExpressionStatement>(std::move(expr), 
                                                 expr->m_line, expr->m_column);
}

std::unique_ptr<Statement> Parser::parse_function_declaration() {
    Token keyword = previous();
    Token name = consume(TokenType::IDENTIFIER, "Expected function name");
    
    consume(TokenType::LPAREN, "Expected '(' after function name");
    auto params = parse_parameters();
    consume(TokenType::RPAREN, "Expected ')' after parameters");
    
    consume(TokenType::LBRACE, "Expected '{' before function body");
    auto body_statements = parse_block();
    consume(TokenType::RBRACE, "Expected '}' after function body");
    
    auto body = std::make_unique<BlockStatement>(std::move(body_statements), 
                                                  keyword.get_line(), keyword.get_column());
    
    return std::make_unique<FunctionDeclaration>(name.get_value(), std::move(params),
                                                 std::move(body), keyword.get_line(), keyword.get_column());
}

std::unique_ptr<Statement> Parser::parse_return_statement() {
    Token keyword = previous();
    std::unique_ptr<Expression> value = nullptr;
    
    if (!check(TokenType::SEMICOLON)) {
        value = parse_expression();
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after return value");
    
    return std::make_unique<ReturnStatement>(std::move(value), 
                                             keyword.get_line(), keyword.get_column());
}

// ========== Парсинг выражений ==========

std::unique_ptr<Expression> Parser::parse_expression() {
    return parse_assignment();
}

std::unique_ptr<Expression> Parser::parse_assignment() {
    auto expr = parse_logical_or();
    
    if (match(TokenType::EQ)) {
        Token equals = previous();
        auto value = parse_assignment();
        
        if (auto var_expr = dynamic_cast<VariableExpression*>(expr.get())) {
            return std::make_unique<AssignExpression>(var_expr->m_name, std::move(value),
                                                      equals.get_line(), equals.get_column());
        }
        
        error(equals, "Invalid assignment target");
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parse_logical_or() {
    auto expr = parse_logical_and();
    
    while (match(TokenType::OR)) {
        Token op = previous();
        auto right = parse_logical_and();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op.get_type(),
                                                  std::move(right), op.get_line(), op.get_column());
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parse_logical_and() {
    auto expr = parse_equality();
    
    while (match(TokenType::AND)) {
        Token op = previous();
        auto right = parse_equality();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op.get_type(),
                                                  std::move(right), op.get_line(), op.get_column());
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parse_equality() {
    auto expr = parse_comparison();
    
    while (match({TokenType::EQEQ, TokenType::NEQ})) {
        Token op = previous();
        auto right = parse_comparison();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op.get_type(),
                                                  std::move(right), op.get_line(), op.get_column());
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parse_comparison() {
    auto expr = parse_term();
    
    while (match({TokenType::LT, TokenType::GT, TokenType::LTEQ, TokenType::GTEQ})) {
        Token op = previous();
        auto right = parse_term();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op.get_type(),
                                                  std::move(right), op.get_line(), op.get_column());
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parse_term() {
    auto expr = parse_factor();
    
    while (match({TokenType::PLUS, TokenType::MINUS})) {
        Token op = previous();
        auto right = parse_factor();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op.get_type(),
                                                  std::move(right), op.get_line(), op.get_column());
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parse_factor() {
    auto expr = parse_unary();
    
    while (match({TokenType::STAR, TokenType::SLASH})) {
        Token op = previous();
        auto right = parse_unary();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op.get_type(),
                                                  std::move(right), op.get_line(), op.get_column());
    }
    
    return expr;
}

std::unique_ptr<Expression> Parser::parse_unary() {
    if (match({TokenType::EXCL, TokenType::MINUS})) {
        Token op = previous();
        auto right = parse_unary();
        return std::make_unique<UnaryExpression>(op.get_type(), std::move(right),
                                                 op.get_line(), op.get_column());
    }
    
    return parse_primary();
}

std::unique_ptr<Expression> Parser::parse_primary() {
    if (match(TokenType::NUMBER)) {
        Token token = previous();
        double value = std::stod(token.get_value());
        return std::make_unique<NumberExpression>(value, token.get_line(), token.get_column());
    }
    
    if (match(TokenType::STRING)) {
        Token token = previous();
        return std::make_unique<StringExpression>(token.get_value(), token.get_line(), token.get_column());
    }
    
    if (match(TokenType::KW_TRUE)) {
        Token token = previous();
        return std::make_unique<BooleanExpression>(true, token.get_line(), token.get_column());
    }
    
    if (match(TokenType::KW_FALSE)) {
        Token token = previous();
        return std::make_unique<BooleanExpression>(false, token.get_line(), token.get_column());
    }
    
    if (match(TokenType::LBRACKET)) {
        return parse_array_literal();
    }
    
    if (match(TokenType::IDENTIFIER)) {
        Token token = previous();
        if (check(TokenType::LPAREN)) {
            return parse_call_expression(token);
        }
        if (check(TokenType::LBRACKET)) {
            auto var = std::make_unique<VariableExpression>(token.get_value(), token.get_line(), token.get_column());
            return parse_index_expression(std::move(var));
        }
        return std::make_unique<VariableExpression>(token.get_value(), token.get_line(), token.get_column());
    }
    
    if (match(TokenType::LPAREN)) {
        auto expr = parse_expression();
        consume(TokenType::RPAREN, "Expected ')' after expression");
        return expr;
    }
    
    error(peek(), "Expected expression");
    return nullptr;
}

std::unique_ptr<Expression> Parser::parse_array_literal() {
    int line = previous().get_line();
    int column = previous().get_column();
    std::vector<std::unique_ptr<Expression>> elements;
    
    if (!check(TokenType::RBRACKET)) {
        elements.push_back(parse_expression());
        while (match(TokenType::COMMA)) {
            elements.push_back(parse_expression());
        }
    }
    
    consume(TokenType::RBRACKET, "Expected ']' after array elements");
    return std::make_unique<ArrayExpression>(std::move(elements), line, column);
}

std::unique_ptr<Expression> Parser::parse_index_expression(std::unique_ptr<Expression> array) {
    int line = previous().get_line();
    int column = previous().get_column();
    consume(TokenType::LBRACKET, "Expected '[' after array name");
    auto index = parse_expression();
    consume(TokenType::RBRACKET, "Expected ']' after index");
    
    if (match(TokenType::EQ)) {
        auto value = parse_expression();
        if (auto var_expr = dynamic_cast<VariableExpression*>(array.get())) {
            return std::make_unique<IndexAssignExpression>(var_expr->m_name, std::move(index),
                                                           std::move(value), line, column);
        }
    }
    
    return std::make_unique<IndexExpression>(std::move(array), std::move(index), line, column);
}

std::unique_ptr<Expression> Parser::parse_call_expression(const Token& name) {
    consume(TokenType::LPAREN, "Expected '(' after function name");
    auto arguments = parse_arguments();
    consume(TokenType::RPAREN, "Expected ')' after arguments");
    
    return std::make_unique<CallExpression>(name.get_value(), std::move(arguments),
                                            name.get_line(), name.get_column());
}

std::vector<std::string> Parser::parse_parameters() {
    std::vector<std::string> params;
    
    if (!check(TokenType::RPAREN)) {
        Token param = consume(TokenType::IDENTIFIER, "Expected parameter name");
        params.push_back(param.get_value());
        
        while (match(TokenType::COMMA)) {
            param = consume(TokenType::IDENTIFIER, "Expected parameter name");
            params.push_back(param.get_value());
        }
    }
    
    return params;
}

std::vector<std::unique_ptr<Expression>> Parser::parse_arguments() {
    std::vector<std::unique_ptr<Expression>> args;
    
    if (!check(TokenType::RPAREN)) {
        args.push_back(parse_expression());
        while (match(TokenType::COMMA)) {
            args.push_back(parse_expression());
        }
    }
    
    return args;
}

std::vector<std::unique_ptr<Statement>> Parser::parse_block() {
    std::vector<std::unique_ptr<Statement>> statements;
    
    while (!check(TokenType::RBRACE) && !is_at_end()) {
        statements.push_back(parse_declaration());
    }
    
    return statements;
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