#pragma once

#include <memory> 
#include <vector> 
#include <string>
#include <unordered_map>
#include "AST.h" 
#include "TypeInfo.h"

class Symbol {
private: 
    std::string m_name;
    TypeInfo m_type;
    bool m_is_used = false;
    bool m_is_param = false;

public:
    Symbol(const std::string& name, const TypeInfo& type)
        : m_name(name) 
        , m_type(type) {}

    const std::string& get_name() const { return m_name; }
    const TypeInfo& get_type() const { return m_type; }
    TypeInfo& get_type() { return m_type; }
    bool is_used() const { return m_is_used; }
    bool is_param() const { return m_is_param; }
    
    void set_used(bool used) { m_is_used = used; }
    void set_param(bool param) { m_is_param = param; }
};


class Scope {
private: 
    std::unordered_map<std::string, Symbol> m_symbols;
    Scope* m_parent = nullptr;

public: 
    explicit Scope(Scope* parent = nullptr) 
        : m_parent(parent) {}
   
        bool define(const std::string& name, const TypeInfo& type);
    Symbol* lookup(const std::string& name);
    bool is_defined_locally(const std::string& name) const;
    Scope* get_parent() { return m_parent; }
    const std::unordered_map<std::string, Symbol>& get_symbols() const { return m_symbols; }
};

class SemanticAnalyzer {
private:
    std::vector<std::string> m_errors;
    std::vector<std::unique_ptr<Scope>> m_scope_storage;
    Scope* m_current_scope = nullptr;
    std::string m_current_function;
    bool m_is_in_function = false;

public:
    SemanticAnalyzer();
    
    void analyze(const std::vector<std::unique_ptr<Statement>>& statements);
    const std::vector<std::string>& get_errors() const { return m_errors; }
    bool has_errors() const { return !m_errors.empty(); }
    
private:
    void visit_statement(const Statement* stmt);
    ValueType visit_expression(const Expression* expr);
    
    void visit_var_statement(const VarStatement* stmt);
    void visit_print_statement(const PrintStatement* stmt);
    void visit_if_statement(const IfStatement* stmt);
    void visit_while_statement(const WhileStatement* stmt);
    void visit_block_statement(const BlockStatement* stmt);
    void visit_expression_statement(const ExpressionStatement* stmt);
    void visit_function_declaration(const FunctionDeclaration* stmt);
    void visit_return_statement(const ReturnStatement* stmt);
    
    ValueType visit_number_expression(const NumberExpression* expr);
    ValueType visit_string_expression(const StringExpression* expr);
    ValueType visit_boolean_expression(const BooleanExpression* expr);
    ValueType visit_variable_expression(const VariableExpression* expr);
    ValueType visit_binary_expression(const BinaryExpression* expr);
    ValueType visit_unary_expression(const UnaryExpression* expr);
    ValueType visit_assign_expression(const AssignExpression* expr);
    ValueType visit_array_expression(const ArrayExpression* expr);
    ValueType visit_index_expression(const IndexExpression* expr);
    ValueType visit_index_assign_expression(const IndexAssignExpression* expr);
    ValueType visit_call_expression(const CallExpression* expr);
    
    void add_error(int line, int column, const std::string& message, bool is_warning = false);
    void enter_scope();
    void exit_scope();
    void check_unused_variables();
    
    ValueType get_binary_result_type(TokenType op, ValueType left, ValueType right, int line, int column);
};

