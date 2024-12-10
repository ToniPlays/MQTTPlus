
#include "SQLQueryBuilder.h"

#include "spdlog/fmt/fmt.h"

#include <sstream>

namespace MQTTPlus
{
    std::string SQLQueryBuilder::CreateQuery()
    {
        std::string type = GetQueryType();
        std::string fields = FormatFields();
        return fmt::format("{} {} FROM {}", type, fields, m_Query.Table);
    }

    std::string SQLQueryBuilder::GetQueryType() const
    {
        switch (m_Query.Type)
        {
            case SQLQueryType::Create: return "INSERT INTO";
            case SQLQueryType::Select: return "SELECT";
            case SQLQueryType::Update: return "UPDATE";
            case SQLQueryType::Delete: return "DELETE";
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
            {
                result << " AS " << field.Alias;
            }
            if(i < m_Query.Fields.size() - 1)
                result << ", ";
        }
        return result.str();
    }
}