#pragma once

#include <stdexcept>
#include <sqlite3.h>

namespace SQLite {
	class SQLiteError;
}

class SQLite::SQLiteError :public std::runtime_error
{
public:
	SQLiteError(sqlite3* dbHandle) :
		std::runtime_error(sqlite3_errmsg(dbHandle))
	{
	}

	SQLiteError(int rc) :std::runtime_error(sqlite3_errstr(rc))
	{
	}

	static void checkError(sqlite3* dbHandle, int rc)
	{
		if (rc != SQLITE_OK)
			throw SQLiteError(dbHandle);
	}

	static void checkError(int rc)
	{
		if (rc != SQLITE_OK)
			throw SQLiteError(rc);
	}
};

