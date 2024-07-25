#pragma once

#include <iostream>
#include <variant>
#include <vector>
#include <spdlog/fmt/fmt.h>

#include <nlohmann/json.hpp>

namespace MQTTPlus::API {

    using json = nlohmann::json;

    class BaseField {
    public:
        
    };
    template<typename T>
    class Field : public BaseField
    {
    public:
        Field() = default;
        Field(T value) : m_Value(value) {}
        
        T Value() const { return m_Value; }
        
        Field<T>& operator= (T lh)
        {
            m_Value = lh;
            return *this;
        }
        
    private:
        T m_Value = T();
    };
    template<typename... Types>
    class Expandable : public BaseField
    {
    public:
        Expandable();
        template <typename T>
        Expandable(T&& value) : m_Value(std::forward<T>(value)) {}
        
        std::variant<Types...> Value() const { return m_Value; }
        
        template <typename T>
        Expandable& operator=(T&& value) {
            m_Value = std::forward<T>(value);
            return *this;
        }
    private:
        std::variant<Types...> m_Value;
    };

    template<typename T>
    class Array : public BaseField
    {
    public:
        Array() = default;
        Array(std::vector<T> values) : m_Values(values) {}
        
        std::vector<T> Values() const { return m_Values; }
        
        Array<T>& operator= (std::vector<T> lh)
        {
            m_Values = lh;
            return *this;
        }
        
    private:
        std::vector<T> m_Values;
    };
    
    template<typename T>
    static void to_json(json& j, const Field<T>& field)
    {
        T value = field.Value();
        to_json(j, value);
    }
    template<typename... T>
    static void to_json(json& j, const Expandable<T...>& field)
    {
        std::visit([value = field.Value(), &j](auto&& arg) mutable {
            using Type = std::decay_t<decltype(arg)>;
            try {
                Type val = std::get<Type>(value);
                to_json(j, val);
                return;
            } catch(std::bad_variant_access& e)
            {
                
            }
        }, field.Value());
    }
    template<typename T>
    static void to_json(json& j, const Array<T>& field)
    {
        auto arr = json::array();
        for(uint32_t i = 0; i < field.Values().size(); i++)
            arr.push_back(field.Values()[i]);
        j += arr;
    }
}