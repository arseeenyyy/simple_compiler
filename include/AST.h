#pragma once

#include <memory>
#include <vector>
#include <string>
#include "TokenType.h"

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual std::string to_string() const = 0;
};

class Expression : public ASTNode {
public:
    int m_line;
    int m_column;

    Expression(int line, int column);
};

class Statement : public ASTNode {
public:
    int m_line;
    int m_column;

    Statement(int line, int column);
};


class NumberExpression : public Expression {
public:
    double m_value;

    NumberExpression(double value, int line, int column);
    std::string to_string() const override;
};

class StringExpression : public Expression {
public:
    std::string m_value;

    StringExpression(const std::string& value, int line, int column);
    StringExpression(std::string&& value, int line, int column);
    std::string to_string() const override;
};

class BooleanExpression : public Expression {
public:
    bool m_value;

    BooleanExpression(bool value, int line, int column);
    std::string to_string() const override;
};

class VariableExpression : public Expression {
public:
    std::string m_name;

    VariableExpression(const std::string& name, int line, int column);
    VariableExpression(std::string&& name, int line, int column);
    std::string to_string() const override;
};

class BinaryExpression : public Expression {
public:
    std::unique_ptr<Expression> m_left;
    TokenType m_operator;
    std::unique_ptr<Expression> m_right;

    BinaryExpression(std::unique_ptr<Expression> left, TokenType op,
                     std::unique_ptr<Expression> right, int line, int column);
    std::string to_string() const override;
};

class UnaryExpression : public Expression {
public:
    TokenType m_operator;
    std::unique_ptr<Expression> m_right;

    UnaryExpression(TokenType op, std::unique_ptr<Expression> right, int line, int column);
    std::string to_string() const override;
};

class AssignExpression : public Expression {
public:
    std::string m_name;
    std::unique_ptr<Expression> m_value;

    AssignExpression(const std::string& name, std::unique_ptr<Expression> value, int line, int column);
    AssignExpression(std::string&& name, std::unique_ptr<Expression> value, int line, int column);
    std::string to_string() const override;
};

class ArrayExpression : public Expression {
public:
    std::vector<std::unique_ptr<Expression>> m_elements;

    ArrayExpression(std::vector<std::unique_ptr<Expression>> elements, int line, int column);
    std::string to_string() const override;
};

class IndexExpression : public Expression {
public:
    std::unique_ptr<Expression> m_array;
    std::unique_ptr<Expression> m_index;

    IndexExpression(std::unique_ptr<Expression> array, std::unique_ptr<Expression> index, int line, int column);
    std::string to_string() const override;
};

class IndexAssignExpression : public Expression {
public:
    std::string m_array;
    std::unique_ptr<Expression> m_index;
    std::unique_ptr<Expression> m_value;

    IndexAssignExpression(const std::string& array, std::unique_ptr<Expression> index,
                          std::unique_ptr<Expression> value, int line, int column);
    IndexAssignExpression(std::string&& array, std::unique_ptr<Expression> index,
                          std::unique_ptr<Expression> value, int line, int column);
    std::string to_string() const override;
};

class CallExpression : public Expression {
public:
    std::string m_callee;
    std::vector<std::unique_ptr<Expression>> m_arguments;

    CallExpression(const std::string& callee, std::vector<std::unique_ptr<Expression>> arguments, int line, int column);
    CallExpression(std::string&& callee, std::vector<std::unique_ptr<Expression>> arguments, int line, int column);
    std::string to_string() const override;
};


class ExpressionStatement : public Statement {
public:
    std::unique_ptr<Expression> m_expression;

    ExpressionStatement(std::unique_ptr<Expression> expression, int line, int column);
    std::string to_string() const override;
};

class PrintStatement : public Statement {
public:
    std::unique_ptr<Expression> m_expression;

    PrintStatement(std::unique_ptr<Expression> expression, int line, int column);
    std::string to_string() const override;
};

class VarStatement : public Statement {
public:
    std::string m_name;
    std::unique_ptr<Expression> m_initializer;

    VarStatement(const std::string& name, std::unique_ptr<Expression> initializer, int line, int column);
    VarStatement(std::string&& name, std::unique_ptr<Expression> initializer, int line, int column);
    std::string to_string() const override;
};

class BlockStatement : public Statement {
public:
    std::vector<std::unique_ptr<Statement>> m_statements;

    BlockStatement(std::vector<std::unique_ptr<Statement>> statements, int line, int column);
    std::string to_string() const override;
};

class IfStatement : public Statement {
public:
    std::unique_ptr<Expression> m_condition;
    std::unique_ptr<Statement> m_then_branch;
    std::unique_ptr<Statement> m_else_branch;

    IfStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> then_branch,
                std::unique_ptr<Statement> else_branch, int line, int column);
    std::string to_string() const override;
};

class WhileStatement : public Statement {
public:
    std::unique_ptr<Expression> m_condition;
    std::unique_ptr<Statement> m_body;

    WhileStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> body, int line, int column);
    std::string to_string() const override;
};

class FunctionDeclaration : public Statement {
public:
    std::string m_name;
    std::vector<std::string> m_params;
    std::unique_ptr<BlockStatement> m_body;

    FunctionDeclaration(const std::string& name, std::vector<std::string> params,
                        std::unique_ptr<BlockStatement> body, int line, int column);
    FunctionDeclaration(std::string&& name, std::vector<std::string> params,
                        std::unique_ptr<BlockStatement> body, int line, int column);
    std::string to_string() const override;
};

class ReturnStatement : public Statement {
public:
    std::unique_ptr<Expression> m_value;

    ReturnStatement(std::unique_ptr<Expression> value, int line, int column);
    std::string to_string() const override;
};
