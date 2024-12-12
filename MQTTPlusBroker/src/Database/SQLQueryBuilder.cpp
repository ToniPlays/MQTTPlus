
#include "SQLQueryBuilder.h"

#include "spdlog/fmt/fmt.h"

#include "Core/Logger.h"
#include <sstream>

namespace MQTTPlus
{
    std::string SQLQueryBuilder::CreateQuery()
    {
        switch (m_Query.Type)
        {
            case SQLQueryType::Select:
            {
                std::string filters = FormatFilters();
                if(!filters.empty())
                    filters = "WHERE " + filters;
                return fmt::format("SELECT {} FROM {} {} {}", FormatFields(), m_Query.Table, FormatJoins(), filters);
            }
            case SQLQueryType::Insert:
            {
                std::string values = FormatValues();
                return fmt::format("INSERT INTO {} ({}) VALUES ({})", m_Query.Table, FormatFields(), values);
            }
            case SQLQueryType::Update:
            {
                std::string filters = FormatFilters();
                if(!filters.empty())
                    filters = "WHERE " + filters;

                return fmt::format("UPDATE {} SET {} {}", m_Query.Table, FormatSetValues(), filters);
            }
        }
    }

    std::string SQLQueryBuilder::FormatFields()
    {
        std::stringstream result;

        for(uint32_t i = 0; i < m_Query.Fields.size(); i++)
        {
            auto& field = m_Query.Fields[i];
            result << field.Name;

            if(!field.Alias.empty())
                result << " AS " << field.Alias;

            if(i < m_Query.Fields.size() - 1)
                result << ", ";
        }
        return result.str();
    }

    std::string SQLQueryBuilder::FormatValues()
    {
        std::stringstream result;

        for(uint32_t i = 0; i < m_Query.Values.size(); i++)
        {
            auto& value = m_Query.Values[i];
            result << value.Value;

            if(i < m_Query.Fields.size() - 1)
                result << ", ";
        }
        return result.str();
    }

    std::string SQLQueryBuilder::FormatJoins()
    {
        std::stringstream result;

        for(uint32_t i = 0; i < m_Query.Joins.size(); i++)
        {
            auto& join = m_Query.Joins[i];
            result << fmt::format("{} {} ON {} = {} ", GetJoinType(join.Type), join.Table, join.From, join.To);
        }
        return result.str();
    }

    std::string SQLQueryBuilder::FormatSetValues()
    {
        if(m_Query.Fields.size() != m_Query.Values.size())
        {
            MQP_ERROR("Cannot format SetValues, not matching");
            return "";
        }

        std::stringstream result;

        for(uint32_t i = 0; i < m_Query.Fields.size(); i++)
        {
            auto& field = m_Query.Fields[i];
            auto& value = m_Query.Values[i];
            result << fmt::format("{} = {}", field.Name, value.Value);

            if(i < m_Query.Fields.size() - 1)
                result << ", ";
        }
        return result.str();
    }

    std::string SQLQueryBuilder::FormatFilters()
    {
        std::stringstream result;

        for(uint32_t i = 0; i < m_Query.Filters.size(); i++)
        {
            auto& filter = m_Query.Filters[i];
            result << fmt::format("{} {} {}", filter.Field, FieldFilterToString(filter.FieldType), filter.Value);

            if(i < m_Query.Filters.size() - 1)
                result << fmt::format(" {} ", FilterToString(filter.Type));

        }
        return result.str();
    }
    std::string SQLQueryBuilder::GetJoinType(SQLJoinType type)
    {
        switch(type)
        {
            case SQLJoinType::Inner:    return "INNER JOIN";
            case SQLJoinType::Outer:    return "OUTER JOIN";
            case SQLJoinType::Left:     return "LEFT JOIN";
            case SQLJoinType::Right:    return "RIGHT JOIN";
            case SQLJoinType::Full:    return "FULL JOIN";
        }
        return "";
    }
}