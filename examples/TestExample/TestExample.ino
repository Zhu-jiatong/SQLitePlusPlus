/*
 Name:		TestExample.ino
 Created:	5/10/2024 12:16:23 AM
 Author:	zhuji
*/

#include "DbConnection.h"
#include <SD.h>
#include <iostream>

// the setup function runs once when you press reset or power the board
void setup()
{
	Serial.begin(115200);
	try
	{
		if (!SD.begin())
			throw std::runtime_error("SD card failed");

		if (SD.exists("/test.db"))
			SD.remove("/test.db");

		DbConnection db("/sd/test.db");
		db.prepare("CREATE TABLE IF NOT EXISTS test (id INTEGER PRIMARY KEY, username TEXT, password TEXT)").evaluate();
		db.prepare("INSERT INTO test VALUES (?, ?, ?)").bind(1, "Tony", "12345678").evaluate();
		db.prepare("INSERT INTO test VALUES (?, ?, ?)").bind(2, "Tyrion", "20051206").evaluate();
		db.prepare("INSERT INTO test VALUES (?, ?, ?)").bind(3, "hnt", "19960621").evaluate();
		db.prepare("INSERT INTO test VALUES (?, ?, ?)").bind(4, "kgym", "19961222").evaluate();

		SQLStatement stmt = db.prepare("SELECT * FROM test");
		int columnCount = stmt.getColumnCount();
		std::cout << "Column count: " << columnCount << '\n';
		while (stmt.evaluate())
		{
			for (int i = 0; i < columnCount; ++i)
				std::cout << stmt.getColumnName(i) << ": " << stmt.getColumnValue<std::string>(i) << ", ";
			std::cout << '\n';
		}
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
