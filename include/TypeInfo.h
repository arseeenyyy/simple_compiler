#pragma once

#include <string> 
#include <string_view>

enum class ValueType {
    NUMBER,
    STRING, 
    BOOLEAN, 
    ARRAY, 
    UNKNOWN
};

inline std::string_view type_to_string(ValueType type) {
    switch(type) {
        case ValueType::NUMBER: return "number";
        case ValueType::STRING: return "string";
        case ValueType::BOOLEAN: return "boolean";
        case ValueType::ARRAY: return "array";
        default: return "unknown";
    }
}

class TypeInfo {
private: 
    ValueType m_type = ValueType::UNKNOWN;
    bool m_initialized = false;
    bool m_isArray = false;
    ValueType m_elementType = ValueType::UNKNOWN;
    int m_arraySize = 0;

public: 
    TypeInfo() = default;
    explicit TypeInfo(ValueType type, bool is_initialized)
        : m_type(type) 
        , m_initialized(is_initialized) {} 

    TypeInfo(ValueType type, bool is_initialized, ValueType elementType, int arraySize)
        : m_type(type)
        , m_initialized(is_initialized)
        , m_isArray(true)
        , m_elementType(elementType)
        , m_arraySize(arraySize) {}
    
    std::string to_string() const {
        if (m_isArray) {
            return "array<" + std::string(type_to_string(m_elementType)) + ">";
        }
        return std::string(type_to_string(m_type));
    }
    ValueType get_type() const { return m_type; }
    bool is_initialized() const { return m_initialized; }
    bool is_array() const { return m_isArray; }
    ValueType get_element_type() const { return m_elementType; }
    int get_array_size() const { return m_arraySize; }
    bool is_known() const { return m_type != ValueType::UNKNOWN; }
    
    void set_initialized(bool initialized) { m_initialized = initialized; }
    void set_type(ValueType type) { m_type = type; }
    void set_element_type(ValueType type) { m_elementType = type; }
};