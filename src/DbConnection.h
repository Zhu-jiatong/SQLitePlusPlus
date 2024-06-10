#pragma once

#include <string>
#include <sqlite3.h>
#include <mutex>

namespace SQLite {
	class SQLStatement;
	class DbConnection;
}

class SQLite::DbConnection
{
public:
	DbConnection(const std::string& filename);
	~DbConnection() noexcept;

	void close();
	SQLStatement prepare(const std::string& sql);

	friend class SQLStatement;

private:
	sqlite3* m_dbHandle;
	std::mutex m_mutex;
};

