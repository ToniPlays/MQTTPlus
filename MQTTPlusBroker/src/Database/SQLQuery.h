#pragma once

#include "Ref.h"

#include <string>
#include <vector>
#include <iostream>
#include <memory>

#include <mariadb/conncpp.hpp>

#include "spdlog/fmt/fmt.h"
#include "Core/Logger.h"

namespace MQTTPlus
{
    enum class SQLQueryType
    {
        Select,
        Update,
        Insert,
        Delete,
    };

    enum class SQLFieldFilterType
    {
        Equal,
        Greater,
        Less,
        GreaterOrEqual,
        LessOrEqual,
        Not,
    };

    enum class SQLFilterType
    {
        And,
        Or,
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

        SQLQueryField(const std::string& name, const char* alias)
        {
            Name = name;
            Alias = alias;
        }
    };

    struct SQLQueryFieldValue
    {
        friend class SQLQueryBuilder;
        
        template<typename T>
        SQLQueryFieldValue(const T& value, bool isString = true)
        {
            if constexpr (std::is_convertible_v<T, std::string_view>)
            {
                if(isString)
                    Value = fmt::format("'{}'", std::string_view(value));
                else Value = std::string_view(value);
            }
            else Value = std::to_string(value);
        }

        private:
            std::string Value;
    };

    struct SQLQueryFilter
    {
        friend class SQLQueryBuilder;

        std::string Field;
        SQLFieldFilterType FieldType;
        SQLFilterType Type;

        template<typename T>
        SQLQueryFilter(const std::string& field, SQLFieldFilterType type, const T& value, SQLFilterType continuation = SQLFilterType::And)
        {
            Field = field;
            FieldType = type;
            Type = continuation;
            Value = std::to_string(value);
        }

        template<>
        SQLQueryFilter(const std::string& field, SQLFieldFilterType type, const std::string& value, SQLFilterType continuation)
        {
            Field = field;
            FieldType = type;
            Type = continuation;
            Value = fmt::format("'{}'", value);
        }

        private:
            std::string Value;
    };

    enum class SQLJoinType
    {
        Inner,
        Outer,
        Left,
        Right,
        Full
    };

    struct SQLTableJoin
    {
        std::string Table;
        std::string From;
        std::string To;
        SQLJoinType Type;
    };


    struct SQLQuery
    {
        SQLQueryType Type;
        std::vector<SQLQueryField> Fields;
        std::vector<SQLQueryFieldValue> Values;
        std::string Table;
        std::vector<SQLQueryFilter> Filters;
        std::vector<SQLTableJoin> Joins;
    };

    class SQLQueryResult : public RefCount
    {
    public:

        SQLQuery Query;
        sql::ResultSet* Results = nullptr;
        uint32_t Rows = 0;

        SQLQueryResult(sql::PreparedStatement* statement, SQLQuery& query, sql::ResultSet* result) : m_Statement(statement), Query(query), Results(result) {
            Rows = result ? result->rowsCount() : 0;
        }

        ~SQLQueryResult() {
            delete Results;
            delete m_Statement;
        }


        template<typename T>
        T Get(const char* name) const
        {
            MQP_ASSERT(false, "Do not plz");
        }

        template<>
        std::string Get(const char* name) const
        {
            return Results->getString(name).c_str();
        }
        template<>
        uint32_t Get(const char* name) const
        {
            return Results->getUInt(name);
        }

        private:
            sql::PreparedStatement* m_Statement;
    };

    static std::string FieldFilterToString(SQLFieldFilterType type)
    {
        switch(type)
        {
            case SQLFieldFilterType::Equal:             return "=";
            case SQLFieldFilterType::Greater:           return ">";
            case SQLFieldFilterType::Less:              return "<";
            case SQLFieldFilterType::GreaterOrEqual:    return ">=";
            case SQLFieldFilterType::LessOrEqual:       return "<=";
            case SQLFieldFilterType::Not:               return "<>";
        }
    }
    static std::string FilterToString(SQLFilterType type)
    {
        switch(type)
        {
            case SQLFilterType::And:    return "AND";
            case SQLFilterType::Or:    return "Or";
        }
    }

    
}