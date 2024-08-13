#pragma once

#include "SQLStatement.h"

namespace SQLite {
	class ReusableSQLStatement;
}

class SQLite::ReusableSQLStatement
{
public:
	ReusableSQLStatement(SQLStatement& stmt) : m_stmt(stmt) {}
	~ReusableSQLStatement() { m_stmt.reset(); }

	SQLStatement& get() { return m_stmt; }

	SQLStatement& operator*() { return get(); }
	SQLStatement* operator->() { return &m_stmt; }

private:
	SQLStatement& m_stmt;
};
