#include <iostream>
#include <fstream>
#include <sstream>
#include "Lexer.h"
#include "Parser.h"

int main() {
    std::string filename = "tests/test2.txt";
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return 1;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    
    std::cout << "Source Code\n";
    std::cout << source << "\n\n";
    
    std::cout << "Tokens\n";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    
    for (const auto& token : tokens) {
        std::cout << token.to_string() << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "AST\n";
    std::cout << "Number of tokens: " << tokens.size() << std::endl;
    
    try {
        Parser parser(std::move(tokens));
        auto statements = parser.parse();
        
        std::cout << "Number of statements: " << statements.size() << std::endl;
        
        for (const auto& stmt : statements) {
            std::cout << stmt->to_string() << std::endl;
        }
    } catch (const ParseError& e) {
        std::cerr << "Parse error: " << e.what() << std::endl;
    }
    
    return 0;
}