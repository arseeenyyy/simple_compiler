#include "AST.h"

Expression::Expression(int line, int column) 
    : m_line(line)
    , m_column(column) {}

Statement::Statement(int line, int column)
    : m_line(line) 
    , m_column(column) {}

NumberExpression::NumberExpression(double value, int line, int column) 
    : Expression(line, column) 
    , m_value(value) {} 

std::string NumberExpression::to_string() const {
    return "Number(" + std::to_string(m_value) + ")";
}

StringExpression::StringExpression(const std::string& value, int line, int column) 
    : Expression(line, column) 
    , m_value(value) {}

StringExpression::StringExpression(std::string&& value, int line, int column)
    : Expression(line, column) 
    , m_value(std::move(value)) {}

std::string StringExpression::to_string() const {
    return "String(\"" + m_value + "\")"; 
}

BooleanExpression::BooleanExpression(bool value, int line, int column) 
    : Expression(line, column) 
    , m_value(value) {}

std::string BooleanExpression::to_string() const {
    return m_value ? "Boolean(true)" : "Boolean(false)";
}

VariableExpression::VariableExpression(const std::string& name, int line, int column)
    : Expression(line, column)
    , m_name(name) {}

VariableExpression::VariableExpression(std::string&& name, int line, int column)
    : Expression(line, column)
    , m_name(std::move(name)) {}

std::string VariableExpression::to_string() const {
    return "Variable(" + m_name + ")";
}


BinaryExpression::BinaryExpression(std::unique_ptr<Expression> left, TokenType op,
                                   std::unique_ptr<Expression> right, int line, int column)
    : Expression(line, column)
    , m_left(std::move(left))
    , m_operator(op)
    , m_right(std::move(right)) {}

std::string BinaryExpression::to_string() const {
    std::string op_str;
    switch (m_operator) {
        case TokenType::PLUS:  op_str = "+"; break;
        case TokenType::MINUS: op_str = "-"; break;
        case TokenType::STAR:  op_str = "*"; break;
        case TokenType::SLASH: op_str = "/"; break;
        case TokenType::EQEQ:  op_str = "=="; break;
        case TokenType::NEQ:   op_str = "!="; break;
        case TokenType::LT:    op_str = "<"; break;
        case TokenType::GT:    op_str = ">"; break;
        case TokenType::LTEQ:  op_str = "<="; break;
        case TokenType::GTEQ:  op_str = ">="; break;
        case TokenType::AND:   op_str = "&&"; break;
        case TokenType::OR:    op_str = "||"; break;
        default:               op_str = "?"; break;
    }
    return "Binary(" + m_left->to_string() + " " + op_str + " " + m_right->to_string() + ")";
}


UnaryExpression::UnaryExpression(TokenType op, std::unique_ptr<Expression> right, int line, int column)
    : Expression(line, column)
    , m_operator(op)
    , m_right(std::move(right)) {}

std::string UnaryExpression::to_string() const {
    std::string op_str = (m_operator == TokenType::MINUS) ? "-" : "!";
    return "Unary(" + op_str + m_right->to_string() + ")";
}


AssignExpression::AssignExpression(const std::string& name, std::unique_ptr<Expression> value, int line, int column)
    : Expression(line, column)
    , m_name(name)
    , m_value(std::move(value)) {}

AssignExpression::AssignExpression(std::string&& name, std::unique_ptr<Expression> value, int line, int column)
    : Expression(line, column)
    , m_name(std::move(name))
    , m_value(std::move(value)) {}

std::string AssignExpression::to_string() const {
    return "Assign(" + m_name + " = " + m_value->to_string() + ")";
}


ArrayExpression::ArrayExpression(std::vector<std::unique_ptr<Expression>> elements, int line, int column)
    : Expression(line, column)
    , m_elements(std::move(elements)) {}

std::string ArrayExpression::to_string() const {
    std::string result = "Array[";
    for (size_t i = 0; i < m_elements.size(); ++i) {
        if (i > 0) result += ", ";
        result += m_elements[i]->to_string();
    }
    result += "]";
    return result;
}


IndexExpression::IndexExpression(std::unique_ptr<Expression> array, std::unique_ptr<Expression> index, int line, int column)
    : Expression(line, column)
    , m_array(std::move(array))
    , m_index(std::move(index)) {}

std::string IndexExpression::to_string() const {
    return "Index(" + m_array->to_string() + "[" + m_index->to_string() + "])";
}


IndexAssignExpression::IndexAssignExpression(const std::string& array, std::unique_ptr<Expression> index,
                                             std::unique_ptr<Expression> value, int line, int column)
    : Expression(line, column)
    , m_array(array)
    , m_index(std::move(index))
    , m_value(std::move(value)) {}

IndexAssignExpression::IndexAssignExpression(std::string&& array, std::unique_ptr<Expression> index,
                                             std::unique_ptr<Expression> value, int line, int column)
    : Expression(line, column)
    , m_array(std::move(array))
    , m_index(std::move(index))
    , m_value(std::move(value)) {}

std::string IndexAssignExpression::to_string() const {
    return "IndexAssign(" + m_array + "[" + m_index->to_string() + "] = " + m_value->to_string() + ")";
}


CallExpression::CallExpression(const std::string& callee, std::vector<std::unique_ptr<Expression>> arguments, int line, int column)
    : Expression(line, column)
    , m_callee(callee)
    , m_arguments(std::move(arguments)) {}

CallExpression::CallExpression(std::string&& callee, std::vector<std::unique_ptr<Expression>> arguments, int line, int column)
    : Expression(line, column)
    , m_callee(std::move(callee))
    , m_arguments(std::move(arguments)) {}

std::string CallExpression::to_string() const {
    std::string result = "Call(" + m_callee + "(";
    for (size_t i = 0; i < m_arguments.size(); ++i) {
        if (i > 0) result += ", ";
        result += m_arguments[i]->to_string();
    }
    result += "))";
    return result;
}


ExpressionStatement::ExpressionStatement(std::unique_ptr<Expression> expression, int line, int column)
    : Statement(line, column)
    , m_expression(std::move(expression)) {}

std::string ExpressionStatement::to_string() const {
    return "ExpressionStmt(" + m_expression->to_string() + ")";
}

PrintStatement::PrintStatement(std::unique_ptr<Expression> expression, int line, int column)
    : Statement(line, column)
    , m_expression(std::move(expression)) {}

std::string PrintStatement::to_string() const {
    return "PrintStmt(" + m_expression->to_string() + ")";
}


VarStatement::VarStatement(const std::string& name, std::unique_ptr<Expression> initializer, int line, int column)
    : Statement(line, column)
    , m_name(name)
    , m_initializer(std::move(initializer)) {}

VarStatement::VarStatement(std::string&& name, std::unique_ptr<Expression> initializer, int line, int column)
    : Statement(line, column)
    , m_name(std::move(name))
    , m_initializer(std::move(initializer)) {}

std::string VarStatement::to_string() const {
    std::string result = "VarStmt(" + m_name;
    if (m_initializer) {
        result += " = " + m_initializer->to_string();
    }
    result += ")";
    return result;
}


BlockStatement::BlockStatement(std::vector<std::unique_ptr<Statement>> statements, int line, int column)
    : Statement(line, column)
    , m_statements(std::move(statements)) {}

std::string BlockStatement::to_string() const {
    std::string result = "BlockStmt{\n";
    for (const auto& stmt : m_statements) {
        result += "  " + stmt->to_string() + "\n";
    }
    result += "}";
    return result;
}


IfStatement::IfStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> then_branch,
                         std::unique_ptr<Statement> else_branch, int line, int column)
    : Statement(line, column)
    , m_condition(std::move(condition))
    , m_then_branch(std::move(then_branch))
    , m_else_branch(std::move(else_branch)) {}

std::string IfStatement::to_string() const {
    std::string result = "IfStmt(condition=" + m_condition->to_string() + ", then=" + m_then_branch->to_string();
    if (m_else_branch) {
        result += ", else=" + m_else_branch->to_string();
    }
    result += ")";
    return result;
}


WhileStatement::WhileStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> body, int line, int column)
    : Statement(line, column)
    , m_condition(std::move(condition))
    , m_body(std::move(body)) {}

std::string WhileStatement::to_string() const {
    return "WhileStmt(condition=" + m_condition->to_string() + ", body=" + m_body->to_string() + ")";
}


FunctionDeclaration::FunctionDeclaration(const std::string& name, std::vector<std::string> params,
                                         std::unique_ptr<BlockStatement> body, int line, int column)
    : Statement(line, column)
    , m_name(name)
    , m_params(std::move(params))
    , m_body(std::move(body)) {}

FunctionDeclaration::FunctionDeclaration(std::string&& name, std::vector<std::string> params,
                                         std::unique_ptr<BlockStatement> body, int line, int column)
    : Statement(line, column)
    , m_name(std::move(name))
    , m_params(std::move(params))
    , m_body(std::move(body)) {}

std::string FunctionDeclaration::to_string() const {
    std::string result = "Function(" + m_name + "(";
    for (size_t i = 0; i < m_params.size(); ++i) {
        if (i > 0) result += ", ";
        result += m_params[i];
    }
    result += ") " + m_body->to_string() + ")";
    return result;
}


ReturnStatement::ReturnStatement(std::unique_ptr<Expression> value, int line, int column)
    : Statement(line, column)
    , m_value(std::move(value)) {}

std::string ReturnStatement::to_string() const {
    if (m_value) {
        return "ReturnStmt(" + m_value->to_string() + ")";
    }
    return "ReturnStmt()";
}