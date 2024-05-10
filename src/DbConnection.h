#pragma once

#include <string>
#include <sqlite3.h>
#include <mutex>
#include <SQLStatement.h>

class DbConnection
{
public:
	DbConnection(const std::string& filename);
	~DbConnection() noexcept;

	void close();
	SQLStatement prepare(const std::string& sql);

private:
	sqlite3* m_dbHandle;
	std::mutex m_mutex;
};

