#include "DbConnection.h"
#include "SQLiteError.h"
#include "SQLStatement.h"
#include "SQLValue.h"

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

SQLite::SQLValue SQLite::DbConnection::preupdateOld(int col)
{
	SQLValue value;
	int rc = sqlite3_preupdate_old(m_dbHandle, col, &value);
	SQLiteError::checkError(m_dbHandle, rc);
	return value;
}

SQLite::SQLValue SQLite::DbConnection::preupdateNew(int col)
{
	SQLValue value;
	int rc = sqlite3_preupdate_new(m_dbHandle, col, &value);
	SQLiteError::checkError(m_dbHandle, rc);
	return value;
}

size_t SQLite::DbConnection::preupdateCount()
{
	return sqlite3_preupdate_count(m_dbHandle);
}

size_t SQLite::DbConnection::preupdateDepth()
{
	return sqlite3_preupdate_depth(m_dbHandle);
}