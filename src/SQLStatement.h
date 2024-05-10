#pragma once
#include <sqlite3.h>
#include <string>

class SQLStatement
{
public:
	SQLStatement(sqlite3* dbHandle, const std::string& sql);
	~SQLStatement() noexcept;

	void finalise();

	template<typename T, typename... Args>
	SQLStatement& bind(T&& value, Args&&... args)
	{
		bind(std::forward<T>(value));
		bind(std::forward<Args>(args)...);
		return *this;
	}

	bool evaluate();

	template<typename T>
	T getColumn(int index);

private:
	sqlite3_stmt* m_stmt;
	int m_paramIndex = 0;

	void bind(int32_t value);
	void bind(uint32_t value);
	void bind(int64_t value);
	void bind(double value);
	void bind(const std::string& value);
	void bind(const char* value);
	void bind(std::nullptr_t);
};

