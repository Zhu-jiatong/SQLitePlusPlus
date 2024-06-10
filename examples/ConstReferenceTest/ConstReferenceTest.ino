/*
 Name:		ConstReferenceTest.ino
 Created:	5/13/2024 6:45:54 PM
 Author:	zhuji
*/

#include "SQLitePlusPlus.h"
#include <SD.h>
#include <string>
#include <iostream>

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	try
	{
		if (!SD.begin())
			throw std::runtime_error("SD card failed");

		if (SD.exists("/test.db"))
			SD.remove("/test.db");

		SQLite::DbConnection db("/sd/test.db");
		db.prepare("CREATE TABLE IF NOT EXISTS test (username TEXT PRIMARY KEY, password TEXT) WITHOUT ROWID").evaluate();
		
		std::string username = "Tony";
		std::string password = "12345678";
		db.prepare("INSERT INTO test VALUES (?, ?)").bind(username, password).evaluate();

		printTable();
	}
	catch (const std::exception&e)
	{
		std::cerr << e.what() << '\n';
	}
}

// the loop function runs over and over again until power down or reset
void loop() {
  
}

void printTable()
{
	SQLite::DbConnection db("/sd/test.db");
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