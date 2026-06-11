#pragma once

#include <string> 
#include <unordered_map>

enum class TokenType {
    // literals
    NUMBER,
    STRING,
    IDENTIFIER,

    // keywords
    KW_VAR,
    KW_PRINT,
    KW_IF,
    KW_ELSE,
    KW_WHILE,
    KW_TRUE,
    KW_FALSE,
    KW_FUN,
    KW_RETURN,

    // arithmetic ops
    PLUS,       
    MINUS,      
    STAR,       
    SLASH,

    // comparison ops
    EQ,         
    EQEQ,       
    NEQ,        
    LT,         
    GT,         
    LTEQ,       
    GTEQ, 

    // logic ops
    EXCL,       
    AND,        
    OR, 

    // delimeters
    LPAREN,    
    RPAREN,     
    LBRACE,     
    RBRACE,     
    LBRACKET,   
    RBRACKET,   
    SEMICOLON,  
    COMMA,      

    END_OF_FILE,
    UNKNOWN
};

std::string_view token_type_to_string(TokenType type);
