#include "SQLStatement.h"
#include "SQLiteError.h"

SQLStatement::SQLStatement(sqlite3* dbHandle, const std::string& sql)
{
	int rc = sqlite3_prepare_v2(dbHandle, sql.c_str(), -1, &m_stmt, nullptr);
	SQLiteError::checkError(dbHandle, rc);
}

SQLStatement::~SQLStatement() noexcept
{
	finalise();
}

void SQLStatement::finalise()
{
	int rc = sqlite3_finalize(m_stmt);
	SQLiteError::checkError(sqlite3_db_handle(m_stmt), rc);
}

bool SQLStatement::evaluate()
{
	int rc = sqlite3_step(m_stmt);
	if (rc == SQLITE_ROW)
		return true;
	if (rc == SQLITE_DONE)
		return false;
	SQLiteError::checkError(sqlite3_db_handle(m_stmt), rc);
}

void SQLStatement::bind(int32_t value)
{
	int rc = sqlite3_bind_int(m_stmt, ++m_paramIndex, value);
	SQLiteError::checkError(sqlite3_db_handle(m_stmt), rc);
}

void SQLStatement::bind(uint32_t value)
{
	int rc = sqlite3_bind_int64(m_stmt, ++m_paramIndex, value);
	SQLiteError::checkError(sqlite3_db_handle(m_stmt), rc);
}

void SQLStatement::bind(int64_t value)
{
	int rc = sqlite3_bind_int64(m_stmt, ++m_paramIndex, value);
	SQLiteError::checkError(sqlite3_db_handle(m_stmt), rc);
}

void SQLStatement::bind(double value)
{
	int rc = sqlite3_bind_double(m_stmt, ++m_paramIndex, value);
	SQLiteError::checkError(sqlite3_db_handle(m_stmt), rc);
}

void SQLStatement::bind(const std::string& value)
{
	bind(value.c_str());
}

void SQLStatement::bind(const char* value)
{
	int rc = sqlite3_bind_text(m_stmt, ++m_paramIndex, value, -1, SQLITE_TRANSIENT);
	SQLiteError::checkError(sqlite3_db_handle(m_stmt), rc);
}

void SQLStatement::bind(std::nullptr_t)
{
	int rc = sqlite3_bind_null(m_stmt, ++m_paramIndex);
	SQLiteError::checkError(sqlite3_db_handle(m_stmt), rc);
}

template<>
int32_t SQLStatement::getColumn<int32_t>(int index)
{
	return sqlite3_column_int(m_stmt, index);
}

template<>
uint32_t SQLStatement::getColumn<uint32_t>(int index)
{
	return sqlite3_column_int64(m_stmt, index);
}

template<>
int64_t SQLStatement::getColumn<int64_t>(int index)
{
	return sqlite3_column_int64(m_stmt, index);
}

template<>
double SQLStatement::getColumn<double>(int index)
{
	return sqlite3_column_double(m_stmt, index);
}

template<>
std::string SQLStatement::getColumn<std::string>(int index)
{
	const char* text = reinterpret_cast<const char*>(sqlite3_column_text(m_stmt, index));
	return text ? text : "";
}