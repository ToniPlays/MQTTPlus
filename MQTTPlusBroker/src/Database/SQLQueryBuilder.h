#pragma once

#include "SQLQuery.h"
#include <string>

namespace MQTTPlus
{
    class SQLQueryBuilder
    {
    public:
        SQLQueryBuilder(const SQLQuery& query) : m_Query(query) {}

        std::string CreateQuery();
        std::string FormatFields();
        std::string FormatValues();
        std::string FormatJoins();
        std::string FormatSetValues();
        std::string FormatFilters();

        std::string GetJoinType(SQLJoinType type);

    private:
        SQLQuery m_Query;
    };
}