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
public: 
    ValueType m_type = ValueType::UNKNOWN;
    bool m_initialized = false;
    bool m_isArray = false;
    ValueType m_elementType = ValueType::UNKNOWN;
    int m_arraySize = 0;

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
};