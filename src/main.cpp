#include <iostream>
#include <fstream>
#include <sstream>
#include "Lexer.h"

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
    
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    
    for (const auto& token : tokens) {
        std::cout << token.to_string() << std::endl;
    }
    
    return 0;
}