#pragma once

#include "DbConnection.h"

class SQLite::SQLValue
{
public:
	enum class Type
	{
		INTEGER = SQLITE_INTEGER,
		FLOAT = SQLITE_FLOAT,
		BLOB = SQLITE_BLOB,
		NULL_ = SQLITE_NULL,
		TEXT = SQLITE_TEXT
	};

	sqlite3_value** operator&()
	{
		return &m_value;
	}

	template<typename T>
	T get() const;

	size_t size() const;
	Type type() const;

private:
	sqlite3_value* m_value = nullptr;
};
