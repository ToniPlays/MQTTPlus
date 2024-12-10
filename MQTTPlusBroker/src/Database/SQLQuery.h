#pragma once

#include <string>
#include <vector>

#include "spdlog/fmt/fmt.h"

namespace MQTTPlus
{
    enum class SQLQueryType
    {
        Select,
        Update,
        Create,
        Delete,
    };

    enum class SQLQueryFilterType
    {
        Equal,
        Greater,
        Less,
        GreaterOrEqual,
        LessOrEqual,
        Not,
    };

    struct SQLQueryField
    {
        std::string Name;
        std::string Alias;

        SQLQueryField() = default;

        SQLQueryField(const char* name)
        {
            Name = name;
        }
        
        SQLQueryField(const std::string& name)
        {
            Name = name;
        }
    };

    struct SQLQueryFilter
    {
        std::string Field;
        SQLQueryFilterType Type;

        template<typename T>
        SQLQueryFilter(const T& value)
        {
            Value = std::to_string(value);
        }

        template<>
        SQLQueryFilter(const std::string& value)
        {
            Value = fmt::format("'{}'", value);
        }

        private:
            std::string Value;
    };


    struct SQLQuery
    {
        SQLQueryType Type;
        std::vector<SQLQueryField> Fields;
        std::string Table;
    };
}