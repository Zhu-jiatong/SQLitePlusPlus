#include "DbConnection.h"
#include "SQLiteError.h"
#include "SQLStatement.h"

DbConnection::DbConnection(const std::string& filename)
{
	int rc = sqlite3_open(filename.c_str(), &m_dbHandle);
	SQLiteError::checkError(m_dbHandle, rc);
}

DbConnection::~DbConnection() noexcept
{
	close();
}

void DbConnection::close()
{
	int rc = sqlite3_close(m_dbHandle);
	SQLiteError::checkError(m_dbHandle, rc);
}

SQLStatement DbConnection::prepare(const std::string& sql)
{
	return SQLStatement(*this, sql);
}
