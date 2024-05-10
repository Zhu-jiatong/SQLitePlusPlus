#pragma once

#include <stdexcept>
#include <sqlite3.h>

class SQLiteError : public std::runtime_error
{
public:
	SQLiteError(sqlite3* dbHandle) :
		std::runtime_error(sqlite3_errmsg(dbHandle))
	{
	}

	static void checkError(sqlite3* dbHandle, int rc)
	{
		if (rc != SQLITE_OK)
			throw SQLiteError(dbHandle);
	}
};

