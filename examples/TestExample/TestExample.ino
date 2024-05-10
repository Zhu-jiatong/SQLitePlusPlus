/*
 Name:		TestExample.ino
 Created:	5/10/2024 12:16:23 AM
 Author:	zhuji
*/

#include "DbConnection.h"
#include <SD.h>

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
		db.prepare("CREATE TABLE IF NOT EXISTS test (id INTEGER PRIMARY KEY, key TEXT)").evaluate();
		db.prepare("INSERT INTO test VALUES (?, ?)").bind(1, "hello").evaluate();
		db.prepare("INSERT INTO test VALUES (?, ?)").bind(2, "world").evaluate();

		SQLStatement stmt = db.prepare("SELECT * FROM test");
		while (stmt.evaluate())
		{
			Serial.print("id: ");
			Serial.print(stmt.getColumn<int>(0));
			Serial.print(", key: ");
			Serial.println(stmt.getColumn<std::string>(1).c_str());
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
