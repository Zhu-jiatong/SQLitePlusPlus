#pragma once
#include <sqlite3.h>
#include <string>
#include "DbConnection.h"
#include <vector>

class SQLite::SQLStatement
{
public:
	SQLStatement(DbConnection& db, const std::string& sql);
	~SQLStatement() noexcept;

	void finalise();

	template<typename T>
	SQLStatement& bind(T&& value)
	{
		m_bind(std::forward<T>(value));
		return *this;
	}

	template<typename T, typename... Args>
	SQLStatement& bind(T&& value, Args&&... args)
	{
		m_bind(std::forward<T>(value));
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

	void m_bind(int32_t value);
	void m_bind(uint32_t value);
	void m_bind(int64_t value);
	void m_bind(uint64_t value);
	void m_bind(bool value);
	void m_bind(double value);
	void m_bind(const std::string& value);
	void m_bind(const char* value);
	void m_bind(std::nullptr_t);
	void m_bind(const std::vector<uint8_t>& value);
};

