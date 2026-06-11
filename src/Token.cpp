#include <sstream>
#include "Token.h"
#include "TokenType.h"

std::string Token::to_string() const {
    std::ostringstream oss;
    oss << "[" << m_line << ":" << m_column << "] " << token_type_to_string(m_type) << "('" << m_value << "')";
    return oss.str();
}