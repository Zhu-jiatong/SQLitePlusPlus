#pragma once
#include <sqlite3.h>
#include <string>
#include "DbConnection.h"

class SQLStatement
{
public:
	SQLStatement(DbConnection& db, const std::string& sql);
	~SQLStatement() noexcept;

	void finalise();

	template<typename T>
	SQLStatement& bind(T&& value)
	{
		bind(std::forward<T>(value));
		return *this;
	}

	template<typename T, typename... Args>
	SQLStatement& bind(T&& value, Args&&... args)
	{
		bind(std::forward<T>(value));
		bind(std::forward<Args>(args)...);
		return *this;
	}

	bool evaluate();
	void clearBindings();
	void reset();

	template<typename T>
	T getColumnValue(int index);

	int getColumnCount() const;
	std::string getColumnName(int index) const;

private:
	DbConnection& m_db;
	sqlite3_stmt* m_stmt;
	int m_paramIndex = 0;

	void bind(int32_t value);
	void bind(uint32_t value);
	void bind(int64_t value);
	void bind(double value);
	void bind(std::string value);
	void bind(const char* value);
	void bind(std::nullptr_t);
};

