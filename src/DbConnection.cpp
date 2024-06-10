#include "DbConnection.h"
#include "SQLiteError.h"
#include "SQLStatement.h"

SQLite::DbConnection::DbConnection(const std::string& filename)
{
	int rc = sqlite3_open(filename.c_str(), &m_dbHandle);
	SQLiteError::checkError(m_dbHandle, rc);
}

SQLite::DbConnection::~DbConnection() noexcept
{
	close();
}

void SQLite::DbConnection::close()
{
	int rc = sqlite3_close(m_dbHandle);
	SQLiteError::checkError(m_dbHandle, rc);
}

SQLite::SQLStatement SQLite::DbConnection::prepare(const std::string& sql)
{
	return SQLStatement(*this, sql);
}
