/*
 Name:		RowUpdateCallbackTest.ino
 Created:	8/4/2024 11:24:00 AM
 Author:	zhuji
*/

#include "SQLitePlusPlus.h"
#include <SD.h>
#include <iostream>

void printTable(SQLite::DbConnection& db);
void initialiseDatabase(SQLite::DbConnection& db);

// the setup function runs once when you press reset or power the board
void setup()
{
	Serial.begin(115200);

	try
	{
		if (!SD.begin(SS, SPI, 80000000))
			throw std::runtime_error("SD card failed");

		if (SD.exists("/test.db"))
			SD.remove("/test.db");

		SQLite::DbConnection db("/sd/test.db");
		std::cout << "Database created\n";
		initialiseDatabase(db);
		std::cout << "Database initialised\n";
		std::string bindVal = "TonyZ";
		db.beforeRowUpdate<std::string>(updateRowCallback, bindVal);
		std::cout << "Row update callback set\n";
		SQLite::SQLStatement stmt1 = db.prepare("UPDATE Users SET Username = ? WHERE ID = 1");
		stmt1.bind("Tony").evaluate();
		stmt1.reset();
		stmt1.bind("TonyZ").evaluate();

		SQLite::SQLStatement stmtDelete = db.prepare("DELETE FROM Users WHERE ID = 1");
		stmtDelete.evaluate();

		printTable(db);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}

// the loop function runs over and over again until power down or reset
void loop()
{

}

void printTable(SQLite::DbConnection& db)
{
	{
		SQLite::SQLStatement stmt = db.prepare("SELECT * FROM Users");
		int columnCount = stmt.getColumnCount();
		std::cout << "Column count: " << columnCount << '\n';
		while (stmt.evaluate())
		{
			for (int i = 0; i < columnCount; ++i)
				std::cout << stmt.getColumnName(i) << ": " << stmt.getColumnValue<std::string>(i) << ", ";
			std::cout << '\n';
		}
	}

	{
		SQLite::SQLStatement stmt = db.prepare("SELECT * FROM Files");
		int columnCount = stmt.getColumnCount();
		std::cout << "Column count: " << columnCount << '\n';
		while (stmt.evaluate())
		{
			for (int i = 0; i < columnCount; ++i)
				std::cout << stmt.getColumnName(i) << ": " << stmt.getColumnValue<std::string>(i) << ", ";
			std::cout << '\n';
		}
	}
}

void initialiseDatabase(SQLite::DbConnection& db)
{
	db.prepare(
		"CREATE TABLE IF NOT EXISTS Users ("
		"ID INTEGER PRIMARY KEY AUTOINCREMENT,"
		"Username TEXT NOT NULL,"
		"Password TEXT NOT NULL"
		")"
	).evaluate();

	db.prepare(
		"CREATE TABLE IF NOT EXISTS Files ("
		"ID INTEGER PRIMARY KEY AUTOINCREMENT,"
		"Filename TEXT NOT NULL,"
		"OwnerID INTEGER NOT NULL,"
		"FOREIGN KEY (OwnerID) REFERENCES Users(ID) ON DELETE CASCADE"
		")"
	).evaluate();

	{
		SQLite::SQLStatement insertion = db.prepare("INSERT INTO Users (Username, Password) VALUES (?, ?)");
		insertion.bind("Tony", "12345678").evaluate();
		insertion.reset();
		insertion.bind("Tyrion", "20051206").evaluate();
		insertion.reset();
		insertion.bind("hnt", "19960621").evaluate();
		insertion.reset();
		insertion.bind("kgym", "19961222").evaluate();
	}
	{
		SQLite::SQLStatement insertion = db.prepare("INSERT INTO Files (Filename, OwnerID) VALUES (?, ?)");
		insertion.bind<std::string, std::uint32_t>("test.txt", 1).evaluate();
		insertion.reset();
		insertion.bind<std::string, std::uint32_t>("test2.txt", 2).evaluate();
		insertion.reset();
		insertion.bind<std::string, std::uint32_t>("test3.txt", 3).evaluate();
		insertion.reset();
		insertion.bind<std::string, std::uint32_t>("test4.txt", 4).evaluate();
		insertion.reset();
		insertion.bind<std::string, std::uint32_t>("test5.txt", 1).evaluate();
		insertion.reset();
		insertion.bind<std::string, std::uint32_t>("test6.txt", 2).evaluate();
		insertion.reset();
		insertion.bind<std::string, std::uint32_t>("test7.txt", 3).evaluate();
		insertion.reset();
		insertion.bind<std::string, std::uint32_t>("test8.txt", 4).evaluate();
	}

	printTable(db);
}

void updateRowCallback(std::string bind, SQLite::DbConnection& db, SQLite::DbConnection::ColumnUpdateType opType, const std::string& dbName, const std::string& tableName, std::int64_t rowID, std::optional<std::int64_t> newRowID)
{
	std::cout << "Row update callback called\n";
	std::cout << "Operation type: " << static_cast<int>(opType) << '\n';
	std::cout << "Database name: " << dbName << '\n';
	std::cout << "Table name: " << tableName << '\n';
	std::cout << "Row ID: " << rowID << '\n';
	if (newRowID)
		std::cout << "New row ID: " << *newRowID << '\n';
	std::cout << "Bind: " << bind << '\n';
}