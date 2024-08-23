#include "SQLStatement.h"
#include "SQLiteError.h"

SQLite::SQLStatement::SQLStatement(DbConnection& db, const std::string& sql) :
	m_db(db)
{
	std::lock_guard<std::mutex> lock(m_db.m_mutex);
	int rc = sqlite3_prepare_v2(m_db.m_dbHandle, sql.c_str(), -1, &m_stmt, nullptr);
	SQLiteError::checkError(m_db.m_dbHandle, rc);
}

SQLite::SQLStatement::~SQLStatement()
{
	sqlite3_finalize(m_stmt);
}

void SQLite::SQLStatement::finalise()
{
	int rc = sqlite3_finalize(m_stmt);
	SQLiteError::checkError(sqlite3_db_handle(m_stmt), rc);
}

bool SQLite::SQLStatement::evaluate()
{
	std::lock_guard<std::mutex> lock(m_db.m_mutex);
	int rc = sqlite3_step(m_stmt);
	if (rc == SQLITE_ROW)
		return true;
	if (rc == SQLITE_DONE)
		return false;
	SQLiteError::checkError(sqlite3_db_handle(m_stmt), rc);
}

void SQLite::SQLStatement::clearBindings()
{
	int rc = sqlite3_clear_bindings(m_stmt);
	SQLiteError::checkError(sqlite3_db_handle(m_stmt), rc);
	m_paramIndex = 0;
}

void SQLite::SQLStatement::reset()
{
	int rc = sqlite3_reset(m_stmt);
	SQLiteError::checkError(sqlite3_db_handle(m_stmt), rc);
	m_paramIndex = 0;
}

int SQLite::SQLStatement::getColumnCount() const
{
	return sqlite3_column_count(m_stmt);
}

std::string SQLite::SQLStatement::getColumnName(int index) const
{
	const char* name = sqlite3_column_name(m_stmt, index);
	return name ? name : "";
}

void SQLite::SQLStatement::m_bind(int32_t value)
{
	int rc = sqlite3_bind_int(m_stmt, ++m_paramIndex, value);
	SQLiteError::checkError(sqlite3_db_handle(m_stmt), rc);
}

void SQLite::SQLStatement::m_bind(uint32_t value)
{
	int rc = sqlite3_bind_int64(m_stmt, ++m_paramIndex, value);
	SQLiteError::checkError(sqlite3_db_handle(m_stmt), rc);
}

void SQLite::SQLStatement::m_bind(int64_t value)
{
	int rc = sqlite3_bind_int64(m_stmt, ++m_paramIndex, value);
	SQLiteError::checkError(sqlite3_db_handle(m_stmt), rc);
}

void SQLite::SQLStatement::m_bind(uint64_t value)
{
	m_bind(std::to_string(value));
}

void SQLite::SQLStatement::m_bind(bool value)
{
	m_bind(static_cast<int32_t>(value));
}

void SQLite::SQLStatement::m_bind(double value)
{
	int rc = sqlite3_bind_double(m_stmt, ++m_paramIndex, value);
	SQLiteError::checkError(sqlite3_db_handle(m_stmt), rc);
}

void SQLite::SQLStatement::m_bind(const std::string& value)
{
	bind(value.c_str());
}

void SQLite::SQLStatement::m_bind(const char* value)
{
	int rc = sqlite3_bind_text(m_stmt, ++m_paramIndex, value, -1, SQLITE_TRANSIENT);
	SQLiteError::checkError(sqlite3_db_handle(m_stmt), rc);
}

void SQLite::SQLStatement::m_bind(std::nullptr_t)
{
	int rc = sqlite3_bind_null(m_stmt, ++m_paramIndex);
	SQLiteError::checkError(sqlite3_db_handle(m_stmt), rc);
}

void SQLite::SQLStatement::m_bind(const std::vector<uint8_t>& value)
{
	int rc = sqlite3_bind_blob(m_stmt, ++m_paramIndex, value.data(), value.size(), SQLITE_TRANSIENT);
	SQLiteError::checkError(sqlite3_db_handle(m_stmt), rc);
}

template<>
int32_t SQLite::SQLStatement::getColumnValue<int32_t>(int index)
{
	return sqlite3_column_int(m_stmt, index);
}

template<>
uint32_t SQLite::SQLStatement::getColumnValue<uint32_t>(int index)
{
	return sqlite3_column_int64(m_stmt, index);
}

template<>
int64_t SQLite::SQLStatement::getColumnValue<int64_t>(int index)
{
	return sqlite3_column_int64(m_stmt, index);
}

template<>
double SQLite::SQLStatement::getColumnValue<double>(int index)
{
	return sqlite3_column_double(m_stmt, index);
}

template<>
const char* SQLite::SQLStatement::getColumnValue<const char*>(int index)
{
	return reinterpret_cast<const char*>(sqlite3_column_text(m_stmt, index));
}

template<>
std::string SQLite::SQLStatement::getColumnValue<std::string>(int index)
{
	const char* text = getColumnValue<const char*>(index);
	return text ? text : "";
}

template<>
std::vector<uint8_t> SQLite::SQLStatement::getColumnValue<std::vector<uint8_t>>(int index)
{
	const uint8_t* blob = reinterpret_cast<const uint8_t*>(sqlite3_column_blob(m_stmt, index));
	int size = sqlite3_column_bytes(m_stmt, index);
	return { blob, blob + size };
}

template<>
bool SQLite::SQLStatement::getColumnValue<bool>(int index)
{
	return sqlite3_column_int(m_stmt, index);
}

template<>
uint64_t SQLite::SQLStatement::getColumnValue<uint64_t>(int index)
{
	std::string text = getColumnValue<std::string>(index);
	return std::stoull(text);
}