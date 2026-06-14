#include "SemanticAnalyzer.h"
#include <iostream>


bool Scope::define(const std::string& name, const TypeInfo& type) {
    if (m_symbols.find(name) != m_symbols.end()) {
        return false;
    }
    m_symbols.emplace(name, Symbol(name, type));
    return true;
}

Symbol* Scope::lookup(const std::string& name) {
    auto it = m_symbols.find(name);
    if (it != m_symbols.end()) {
        return &it->second;
    }
    if (m_parent) {
        return m_parent->lookup(name);
    }
    return nullptr;
}

bool Scope::is_defined_locally(const std::string& name) const {
    return m_symbols.find(name) != m_symbols.end();
}


SemanticAnalyzer::SemanticAnalyzer() {
    enter_scope();
}

void SemanticAnalyzer::analyze(const std::vector<std::unique_ptr<Statement>>& statements) {
    for (const auto& stmt : statements) {
        visit_statement(stmt.get());
    }
    check_unused_variables();
}

void SemanticAnalyzer::visit_statement(const Statement* stmt) {
    if (auto var = dynamic_cast<const VarStatement*>(stmt)) {
        visit_var_statement(var);
    } else if (auto print = dynamic_cast<const PrintStatement*>(stmt)) {
        visit_print_statement(print);
    } else if (auto if_stmt = dynamic_cast<const IfStatement*>(stmt)) {
        visit_if_statement(if_stmt);
    } else if (auto while_stmt = dynamic_cast<const WhileStatement*>(stmt)) {
        visit_while_statement(while_stmt);
    } else if (auto block = dynamic_cast<const BlockStatement*>(stmt)) {
        visit_block_statement(block);
    } else if (auto expr = dynamic_cast<const ExpressionStatement*>(stmt)) {
        visit_expression_statement(expr);
    } else if (auto func = dynamic_cast<const FunctionDeclaration*>(stmt)) {
        visit_function_declaration(func);
    } else if (auto ret = dynamic_cast<const ReturnStatement*>(stmt)) {
        visit_return_statement(ret);
    }
}

ValueType SemanticAnalyzer::visit_expression(const Expression* expr) {
    if (auto num = dynamic_cast<const NumberExpression*>(expr)) {
        return visit_number_expression(num);
    } else if (auto str = dynamic_cast<const StringExpression*>(expr)) {
        return visit_string_expression(str);
    } else if (auto boolean = dynamic_cast<const BooleanExpression*>(expr)) {
        return visit_boolean_expression(boolean);
    } else if (auto var = dynamic_cast<const VariableExpression*>(expr)) {
        return visit_variable_expression(var);
    } else if (auto binary = dynamic_cast<const BinaryExpression*>(expr)) {
        return visit_binary_expression(binary);
    } else if (auto unary = dynamic_cast<const UnaryExpression*>(expr)) {
        return visit_unary_expression(unary);
    } else if (auto assign = dynamic_cast<const AssignExpression*>(expr)) {
        return visit_assign_expression(assign);
    } else if (auto arr = dynamic_cast<const ArrayExpression*>(expr)) {
        return visit_array_expression(arr);
    } else if (auto idx = dynamic_cast<const IndexExpression*>(expr)) {
        return visit_index_expression(idx);
    } else if (auto idx_assign = dynamic_cast<const IndexAssignExpression*>(expr)) {
        return visit_index_assign_expression(idx_assign);
    } else if (auto call = dynamic_cast<const CallExpression*>(expr)) {
        return visit_call_expression(call);
    }
    return ValueType::UNKNOWN;
}


void SemanticAnalyzer::visit_var_statement(const VarStatement* stmt) {
    TypeInfo var_type;
    
    if (stmt->m_initializer) {
        ValueType init_type = visit_expression(stmt->m_initializer.get());
        
        if (auto arr_expr = dynamic_cast<const ArrayExpression*>(stmt->m_initializer.get())) {
            if (!arr_expr->m_elements.empty()) {
                ValueType elem_type = visit_expression(arr_expr->m_elements[0].get());
                var_type = TypeInfo(ValueType::ARRAY, true, elem_type, arr_expr->m_elements.size());
            } else {
                var_type = TypeInfo(ValueType::ARRAY, true, ValueType::UNKNOWN, 0);
            }
        } else {
            var_type = TypeInfo(init_type, true);
        }
    }
    
    if (!m_current_scope->define(stmt->m_name, var_type)) {
        add_error(stmt->m_line, stmt->m_column,
                  "Variable '" + stmt->m_name + "' already declared in this scope");
    }
}

void SemanticAnalyzer::visit_print_statement(const PrintStatement* stmt) {
    visit_expression(stmt->m_expression.get());
}

void SemanticAnalyzer::visit_if_statement(const IfStatement* stmt) {
    ValueType cond_type = visit_expression(stmt->m_condition.get());
    
    if (cond_type != ValueType::BOOLEAN && cond_type != ValueType::UNKNOWN) {
        add_error(stmt->m_condition->m_line, stmt->m_condition->m_column,
                  "If condition must be boolean, got " + std::string(type_to_string(cond_type)));
    }
    
    visit_statement(stmt->m_then_branch.get());
    
    if (stmt->m_else_branch) {
        visit_statement(stmt->m_else_branch.get());
    }
}

void SemanticAnalyzer::visit_while_statement(const WhileStatement* stmt) {
    ValueType cond_type = visit_expression(stmt->m_condition.get());
    
    if (cond_type != ValueType::BOOLEAN && cond_type != ValueType::UNKNOWN) {
        add_error(stmt->m_condition->m_line, stmt->m_condition->m_column,
                  "While condition must be boolean, got " + std::string(type_to_string(cond_type)));
    }
    
    visit_statement(stmt->m_body.get());
}

void SemanticAnalyzer::visit_block_statement(const BlockStatement* stmt) {
    enter_scope();
    
    for (const auto& inner_stmt : stmt->m_statements) {
        visit_statement(inner_stmt.get());
    }
    
    check_unused_variables();
    exit_scope();
}

void SemanticAnalyzer::visit_expression_statement(const ExpressionStatement* stmt) {
    visit_expression(stmt->m_expression.get());
}

void SemanticAnalyzer::visit_function_declaration(const FunctionDeclaration* stmt) {
    if (m_current_scope->lookup(stmt->m_name)) {
        add_error(stmt->m_line, stmt->m_column,
                  "Function '" + stmt->m_name + "' already declared");
        return;
    }
    
    m_current_scope->define(stmt->m_name, TypeInfo(ValueType::UNKNOWN, true));
    
    enter_scope();
    m_is_in_function = true;
    m_current_function = stmt->m_name;
    
    for (const auto& param : stmt->m_params) {
        m_current_scope->define(param, TypeInfo(ValueType::NUMBER, true));
        auto* sym = m_current_scope->lookup(param);
        if (sym) sym->set_param(true);
    }
    
    visit_statement(stmt->m_body.get());
    
    check_unused_variables();
    exit_scope();
    m_is_in_function = false;
    m_current_function.clear();
}

void SemanticAnalyzer::visit_return_statement(const ReturnStatement* stmt) {
    if (!m_is_in_function) {
        add_error(stmt->m_line, stmt->m_column, "Return statement outside function");
        return;
    }
    
    if (stmt->m_value) {
        visit_expression(stmt->m_value.get());
    }
}


ValueType SemanticAnalyzer::visit_number_expression(const NumberExpression* expr) {
    return ValueType::NUMBER;
}

ValueType SemanticAnalyzer::visit_string_expression(const StringExpression* expr) {
    return ValueType::STRING;
}

ValueType SemanticAnalyzer::visit_boolean_expression(const BooleanExpression* expr) {
    return ValueType::BOOLEAN;
}

ValueType SemanticAnalyzer::visit_variable_expression(const VariableExpression* expr) {
    Symbol* sym = m_current_scope->lookup(expr->m_name);
    
    if (!sym) {
        add_error(expr->m_line, expr->m_column,
                  "Use of undeclared variable '" + expr->m_name + "'");
        return ValueType::UNKNOWN;
    }
    
    sym->set_used(true);
    
    if (!sym->get_type().is_initialized() && !sym->is_param()) {
        add_error(expr->m_line, expr->m_column,
                  "Use of uninitialized variable '" + expr->m_name + "'", true);
    }
    
    if (sym->get_type().is_array()) {
        return sym->get_type().get_element_type();
    }
    
    return sym->get_type().get_type();
}

ValueType SemanticAnalyzer::visit_binary_expression(const BinaryExpression* expr) {
    ValueType left = visit_expression(expr->m_left.get());
    ValueType right = visit_expression(expr->m_right.get());
    
    return get_binary_result_type(expr->m_operator, left, right, expr->m_line, expr->m_column);
}

ValueType SemanticAnalyzer::visit_unary_expression(const UnaryExpression* expr) {
    ValueType right = visit_expression(expr->m_right.get());
    
    if (expr->m_operator == TokenType::MINUS) {
        if (right != ValueType::NUMBER && right != ValueType::UNKNOWN) {
            add_error(expr->m_line, expr->m_column,
                      "Unary minus requires number, got " + std::string(type_to_string(right)));
        }
        return ValueType::NUMBER;
    } else if (expr->m_operator == TokenType::EXCL) {
        if (right != ValueType::BOOLEAN && right != ValueType::UNKNOWN) {
            add_error(expr->m_line, expr->m_column,
                      "Logical not requires boolean, got " + std::string(type_to_string(right)));
        }
        return ValueType::BOOLEAN;
    }
    
    return ValueType::UNKNOWN;
}

ValueType SemanticAnalyzer::visit_assign_expression(const AssignExpression* expr) {
    ValueType value_type = visit_expression(expr->m_value.get());
    
    Symbol* sym = m_current_scope->lookup(expr->m_name);
    
    if (!sym) {
        add_error(expr->m_line, expr->m_column,
                  "Assignment to undeclared variable '" + expr->m_name + "'");
        return value_type;
    }
    
    sym->get_type().set_initialized(true);
    
    return value_type;
}

ValueType SemanticAnalyzer::visit_array_expression(const ArrayExpression* expr) {
    if (expr->m_elements.empty()) {
        add_error(expr->m_line, expr->m_column, "Empty array literal", true);
        return ValueType::ARRAY;
    }
    
    ValueType first_type = visit_expression(expr->m_elements[0].get());
    
    for (size_t i = 1; i < expr->m_elements.size(); ++i) {
        ValueType elem_type = visit_expression(expr->m_elements[i].get());
        if (elem_type != first_type && elem_type != ValueType::UNKNOWN) {
            add_error(expr->m_line, expr->m_column,
                      "Array elements must have the same type");
        }
    }
    
    return ValueType::ARRAY;
}

ValueType SemanticAnalyzer::visit_index_expression(const IndexExpression* expr) {
    ValueType array_type = visit_expression(expr->m_array.get());
    ValueType index_type = visit_expression(expr->m_index.get());
    
    if (index_type != ValueType::NUMBER && index_type != ValueType::UNKNOWN) {
        add_error(expr->m_index->m_line, expr->m_index->m_column,
                  "Array index must be a number");
    }
    
    if (array_type != ValueType::ARRAY && array_type != ValueType::UNKNOWN) {
        add_error(expr->m_array->m_line, expr->m_array->m_column,
                  "Cannot index non-array value");
        return ValueType::UNKNOWN;
    }
    
    if (auto var_expr = dynamic_cast<const VariableExpression*>(expr->m_array.get())) {
        Symbol* sym = m_current_scope->lookup(var_expr->m_name);
        if (sym && sym->get_type().is_array()) {
            return sym->get_type().get_element_type();
        }
    }
    
    return ValueType::UNKNOWN;
}

ValueType SemanticAnalyzer::visit_index_assign_expression(const IndexAssignExpression* expr) {
    ValueType index_type = visit_expression(expr->m_index.get());
    ValueType value_type = visit_expression(expr->m_value.get());
    
    if (index_type != ValueType::NUMBER && index_type != ValueType::UNKNOWN) {
        add_error(expr->m_index->m_line, expr->m_index->m_column,
                  "Array index must be a number");
    }
    
    Symbol* sym = m_current_scope->lookup(expr->m_array);
    if (!sym) {
        add_error(expr->m_line, expr->m_column,
                  "Array '" + expr->m_array + "' not declared");
        return value_type;
    }
    
    if (!sym->get_type().is_array()) {
        add_error(expr->m_line, expr->m_column,
                  "Variable '" + expr->m_array + "' is not an array");
        return value_type;
    }
    
    if (sym->get_type().get_element_type() != ValueType::UNKNOWN && value_type != ValueType::UNKNOWN) {
        if (sym->get_type().get_element_type() != value_type) {
            add_error(expr->m_value->m_line, expr->m_value->m_column,
                      "Type mismatch: cannot assign " + std::string(type_to_string(value_type)) +
                      " to array element of type " + std::string(type_to_string(sym->get_type().get_element_type())));
        }
    }
    
    return value_type;
}

ValueType SemanticAnalyzer::visit_call_expression(const CallExpression* expr) {
    Symbol* sym = m_current_scope->lookup(expr->m_callee);
    
    if (!sym) {
        add_error(expr->m_line, expr->m_column,
                  "Call to undeclared function '" + expr->m_callee + "'");
        return ValueType::UNKNOWN;
    }
    
    for (const auto& arg : expr->m_arguments) {
        visit_expression(arg.get());
    }
    
    return ValueType::NUMBER;
}


void SemanticAnalyzer::add_error(int line, int column, const std::string& message, bool is_warning) {
    std::string prefix = is_warning ? "Warning" : "Error";
    m_errors.push_back("[" + std::to_string(line) + ":" + std::to_string(column) + "] " +
                       prefix + ": " + message);
}

void SemanticAnalyzer::enter_scope() {
    auto new_scope = std::make_unique<Scope>(m_current_scope);
    m_current_scope = new_scope.get();
    m_scope_storage.push_back(std::move(new_scope));
}

void SemanticAnalyzer::exit_scope() {
    if (m_scope_storage.empty()) return;
    m_scope_storage.pop_back();
    m_current_scope = m_scope_storage.empty() ? nullptr : m_scope_storage.back().get();
}

void SemanticAnalyzer::check_unused_variables() {
    for (const auto& [name, symbol] : m_current_scope->get_symbols()) {
        if (!symbol.is_used() && !symbol.is_param()) {
            add_error(0, 0, "Unused variable '" + name + "'", true);
        }
    }
}

ValueType SemanticAnalyzer::get_binary_result_type(TokenType op, ValueType left, ValueType right, int line, int column) {
    if (op == TokenType::PLUS || op == TokenType::MINUS ||
        op == TokenType::STAR || op == TokenType::SLASH) {
        
        if (op == TokenType::PLUS && (left == ValueType::STRING || right == ValueType::STRING)) {
            if (left != ValueType::STRING && right != ValueType::STRING &&
                left != ValueType::UNKNOWN && right != ValueType::UNKNOWN) {
                add_error(line, column,
                          "Cannot mix string and number in addition");
            }
            return ValueType::STRING;
        }
        
        if ((left == ValueType::NUMBER && right == ValueType::NUMBER) ||
            left == ValueType::UNKNOWN || right == ValueType::UNKNOWN) {
            return ValueType::NUMBER;
        }
        
        add_error(line, column,
                  "Operator requires numbers, got " + std::string(type_to_string(left)) +
                  " and " + std::string(type_to_string(right)));
        return ValueType::UNKNOWN;
    }
    
    if (op == TokenType::EQEQ || op == TokenType::NEQ) {
        return ValueType::BOOLEAN;
    }
    
    if (op == TokenType::LT || op == TokenType::GT ||
        op == TokenType::LTEQ || op == TokenType::GTEQ) {
        if ((left == ValueType::NUMBER && right == ValueType::NUMBER) ||
            left == ValueType::UNKNOWN || right == ValueType::UNKNOWN) {
            return ValueType::BOOLEAN;
        }
        add_error(line, column,
                  "Comparison requires numbers, got " + std::string(type_to_string(left)) +
                  " and " + std::string(type_to_string(right)));
        return ValueType::UNKNOWN;
    }
    
    if (op == TokenType::AND || op == TokenType::OR) {
        if ((left == ValueType::BOOLEAN && right == ValueType::BOOLEAN) ||
            left == ValueType::UNKNOWN || right == ValueType::UNKNOWN) {
            return ValueType::BOOLEAN;
        }
        add_error(line, column,
                  "Logical operator requires booleans, got " + std::string(type_to_string(left)) +
                  " and " + std::string(type_to_string(right)));
        return ValueType::UNKNOWN;
    }
    
    return ValueType::UNKNOWN;
}