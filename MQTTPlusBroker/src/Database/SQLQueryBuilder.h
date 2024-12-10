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

        std::string GetQueryType() const;
        std::string FormatFields();

    private:
        SQLQuery m_Query;
    };
}