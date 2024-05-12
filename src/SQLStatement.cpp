#include "SQLStatement.h"
#include "SQLiteError.h"

SQLStatement::SQLStatement(DbConnection& db, const std::string& sql) :
	m_db(db)
{
	std::lock_guard<std::mutex> lock(m_db.m_mutex);
	int rc = sqlite3_prepare_v2(m_db.m_dbHandle, sql.c_str(), -1, &m_stmt, nullptr);
	SQLiteError::checkError(m_db.m_dbHandle, rc);
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
	std::lock_guard<std::mutex> lock(m_db.m_mutex);
	int rc = sqlite3_step(m_stmt);
	if (rc == SQLITE_ROW)
		return true;
	if (rc == SQLITE_DONE)
		return false;
	SQLiteError::checkError(sqlite3_db_handle(m_stmt), rc);
}

void SQLStatement::clearBindings()
{
	int rc = sqlite3_clear_bindings(m_stmt);
	SQLiteError::checkError(sqlite3_db_handle(m_stmt), rc);
	m_paramIndex = 0;
}

void SQLStatement::reset()
{
	int rc = sqlite3_reset(m_stmt);
	SQLiteError::checkError(sqlite3_db_handle(m_stmt), rc);
	m_paramIndex = 0;
}

int SQLStatement::getColumnCount() const
{
	return sqlite3_column_count(m_stmt);
}

std::string SQLStatement::getColumnName(int index) const
{
	const char* name = sqlite3_column_name(m_stmt, index);
	return name ? name : "";
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

void SQLStatement::bind(std::string value)
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
int32_t SQLStatement::getColumnValue<int32_t>(int index)
{
	return sqlite3_column_int(m_stmt, index);
}

template<>
uint32_t SQLStatement::getColumnValue<uint32_t>(int index)
{
	return sqlite3_column_int64(m_stmt, index);
}

template<>
int64_t SQLStatement::getColumnValue<int64_t>(int index)
{
	return sqlite3_column_int64(m_stmt, index);
}

template<>
double SQLStatement::getColumnValue<double>(int index)
{
	return sqlite3_column_double(m_stmt, index);
}

template<>
std::string SQLStatement::getColumnValue<std::string>(int index)
{
	const char* text = reinterpret_cast<const char*>(sqlite3_column_text(m_stmt, index));
	return text ? text : "";
}