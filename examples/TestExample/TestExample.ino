/*
 Name:		TestExample.ino
 Created:	5/10/2024 12:16:23 AM
 Author:	zhuji
*/

#include "SQLitePlusPlus.h"
#include <SD.h>
#include <iostream>

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
		printForeignKeyMode(db);

		db.prepare(
			"CREATE TABLE test ("
			"id INTEGER NOT NULL,"
			"username TEXT,"
			"password TEXT,"
			"capacity TEXT,"
			"read BOOLEAN,"
			"PRIMARY KEY (id AUTOINCREMENT)"
			")"
		).evaluate();

		db.prepare(
			"INSERT INTO test (username, password, capacity, read) "
			"VALUES (?, ?, ?, ?)"
		).bind("Tony", "12345678", UINT64_MAX, false).evaluate();

		db.prepare(
			"INSERT INTO test (username, password, capacity, read) "
			"VALUES (?, ?, ?, ?)"
		).bind("Tyrion", "20051206", 9999999999999999999UL, true).evaluate();

		db.prepare(
			"INSERT INTO test (username, password, capacity, read) "
			"VALUES (?, ?, ?, ?)"
		).bind("hnt", "19960621", 8999999999999999998UL, false).evaluate();

		db.prepare(
			"INSERT INTO test (username, password, capacity, read) "
			"VALUES (?, ?, ?, ?)"
		).bind("kgym", "19961222", 7999999999999999997UL, true).evaluate();

		printTable(db);

		db.prepare(
			"DELETE FROM test WHERE username = ?"
		).bind("Tony").evaluate();

		printTable(db);
	}
	catch (const std::exception& e)
	{
		Serial.println(e.what());
	}
}

// the loop function runs over and over again until power down or reset
void loop()
{

}

void printTable(SQLite::DbConnection& db)
{
	SQLite::SQLStatement stmt = db.prepare("SELECT * FROM test");
	int columnCount = stmt.getColumnCount();
	std::cout << "Column count: " << columnCount << '\n';
	while (stmt.evaluate())
	{
		for (int i = 0; i < columnCount; ++i)
			std::cout << stmt.getColumnName(i) << ": " << stmt.getColumnValue<std::string>(i) << ", ";
		std::cout << '\n';
	}
}

void printForeignKeyMode(SQLite::DbConnection& db)
{
	SQLite::SQLStatement stmt = db.prepare("PRAGMA foreign_keys");
	int columnCount = stmt.getColumnCount();
	std::cout << "ForeignKeyMode: " << '\n';
	while (stmt.evaluate())
	{
		for (int i = 0; i < columnCount; ++i)
			std::cout << stmt.getColumnValue<int32_t>(i) << ", ";
		std::cout << '\n';
	}
}