#include "SQLValue.h"

template<>
std::int32_t SQLite::SQLValue::get<std::int32_t>() const
{
	return sqlite3_value_int(m_value);
}

template<>
std::int64_t SQLite::SQLValue::get<std::int64_t>() const
{
	return sqlite3_value_int64(m_value);
}

template<>
double SQLite::SQLValue::get<double>() const
{
	return sqlite3_value_double(m_value);
}

template<>
const char* SQLite::SQLValue::get<const char*>() const
{
	return reinterpret_cast<const char*>(sqlite3_value_text(m_value));
}

template<>
std::string SQLite::SQLValue::get<std::string>() const
{
	const char* text = get<const char*>();
	return text ? text : "";
}

template<>
std::vector<std::uint8_t> SQLite::SQLValue::get<std::vector<std::uint8_t>>() const
{
	const std::uint8_t* blob = reinterpret_cast<const std::uint8_t*>(sqlite3_value_blob(m_value));
	int size = sqlite3_value_bytes(m_value);
	return { blob, blob + size };
}

size_t SQLite::SQLValue::size() const
{
	return sqlite3_value_bytes(m_value);
}

SQLite::SQLValue::Type SQLite::SQLValue::type() const
{
	return static_cast<Type>(sqlite3_value_type(m_value));
}
