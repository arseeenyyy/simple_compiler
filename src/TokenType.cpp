#include "TokenType.h"

std::string_view token_type_to_string(TokenType type) {
    using namespace std::string_view_literals;

    switch (type) {
        case TokenType::NUMBER:      return "NUMBER"sv;
        case TokenType::STRING:      return "STRING"sv;
        case TokenType::IDENTIFIER:  return "IDENTIFIER"sv;
        case TokenType::KW_VAR:      return "VAR"sv;
        case TokenType::KW_PRINT:    return "PRINT"sv;
        case TokenType::KW_IF:       return "IF"sv;
        case TokenType::KW_ELSE:     return "ELSE"sv;
        case TokenType::KW_WHILE:    return "WHILE"sv;
        case TokenType::KW_TRUE:     return "TRUE"sv;
        case TokenType::KW_FALSE:    return "FALSE"sv;
        case TokenType::KW_FUN:      return "FUN"sv;
        case TokenType::KW_RETURN:   return "RETURN"sv;
        case TokenType::PLUS:        return "PLUS"sv;
        case TokenType::MINUS:       return "MINUS"sv;
        case TokenType::STAR:        return "STAR"sv;
        case TokenType::SLASH:       return "SLASH"sv;
        case TokenType::EQ:          return "EQ"sv;
        case TokenType::EQEQ:        return "EQEQ"sv;
        case TokenType::NEQ:         return "NEQ"sv;
        case TokenType::LT:          return "LT"sv;
        case TokenType::GT:          return "GT"sv;
        case TokenType::LTEQ:        return "LTEQ"sv;
        case TokenType::GTEQ:        return "GTEQ"sv;
        case TokenType::EXCL:        return "EXCL"sv;
        case TokenType::AND:         return "AND"sv;
        case TokenType::OR:          return "OR"sv;
        case TokenType::LPAREN:      return "LPAREN"sv;
        case TokenType::RPAREN:      return "RPAREN"sv;
        case TokenType::LBRACE:      return "LBRACE"sv;
        case TokenType::RBRACE:      return "RBRACE"sv;
        case TokenType::LBRACKET:    return "LBRACKET"sv;
        case TokenType::RBRACKET:    return "RBRACKET"sv;
        case TokenType::SEMICOLON:   return "SEMICOLON"sv;
        case TokenType::COMMA:       return "COMMA"sv;
        case TokenType::END_OF_FILE: return "EOF"sv;
        default:                     return "UNKNOWN"sv;
    }
}
