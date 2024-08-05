#pragma once

#define SQLITE_ENABLE_PREUPDATE_HOOK

#include <string>
#include <sqlite3.h>
#include <mutex>
#include <functional>
#include <optional>
#include <memory>

namespace SQLite {
	class SQLStatement;
	class DbConnection;
	class SQLValue;
}

class SQLite::DbConnection
{
public:
	enum class ColumnUpdateType
	{
		INSERT = SQLITE_INSERT,
		UPDATE = SQLITE_UPDATE,
		DELETE = SQLITE_DELETE
	};
	template<typename T>
	using RowUpdateCallback = std::function<
		void(
			T& bindParam,
			DbConnection& db,
			ColumnUpdateType operationType,
			const std::string& dbName,
			const std::string& tableName,
			std::int64_t rowid,
			std::optional<std::int64_t> newRowid
			)>;

	DbConnection(const std::string& filename);
	~DbConnection() noexcept;

	void close();
	SQLStatement prepare(const std::string& sql);

	template<typename T>
	std::optional<std::shared_ptr<T>> beforeRowUpdate(RowUpdateCallback<T> callback, T& bindParam);

	SQLValue preupdateOld(int col);
	SQLValue preupdateNew(int col);
	size_t preupdateCount();
	size_t preupdateDepth();

	friend class SQLStatement;

private:
	sqlite3* m_dbHandle;
	static inline std::mutex m_mutex; // global mutex necessary? TODO: static initialisation untested!
};

template<typename T>
inline std::optional<std::shared_ptr<T>> SQLite::DbConnection::beforeRowUpdate(RowUpdateCallback<T> callback, T& bindParam)
{
	struct Context
	{
		RowUpdateCallback<T> callback;
		DbConnection* db;
		T* bindParam;
	};

	Context* context = new Context{ callback, this, &bindParam };

	auto callbackWrapper = [](
		void* pCtx,
		sqlite3* db,
		int op,
		char const* zDb,
		char const* zName,
		sqlite3_int64 iKey1,
		sqlite3_int64 iKey2
		)
		{
			Context* context = static_cast<Context*>(pCtx);
			DbConnection* dbConn = context->db;
			T* bindParam = context->bindParam;
			ColumnUpdateType opType = static_cast<ColumnUpdateType>(op);

			if (op == SQLITE_INSERT || op == SQLITE_UPDATE)
				context->callback(*bindParam, *dbConn, opType, zDb, zName, iKey1, iKey2);
			else
				context->callback(*bindParam, *dbConn, opType, zDb, zName, iKey1, std::nullopt);
		};

	void* prevBindVoidPtr = sqlite3_preupdate_hook(m_dbHandle, callbackWrapper, context);
	if (prevBindVoidPtr == nullptr)
		return std::nullopt;

	T* prevBindPtr = static_cast<T*>(prevBindVoidPtr);
	return std::shared_ptr<T>(prevBindPtr);
}
