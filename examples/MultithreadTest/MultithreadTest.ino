/*
 Name:		MultithreadTest.ino
 Created:	5/12/2024 4:39:24 AM
 Author:	zhuji
*/

#include "SQLitePlusPlus.h"
#include <SD.h>
#include <iostream>
#include <thread>
#include <esp_pthread.h>

// the setup function runs once when you press reset or power the board
void setup()
{
	Serial.begin(115200);

	esp_pthread_cfg_t cfg = esp_pthread_get_default_config();
	cfg.stack_size = 4096 * 2;
	esp_pthread_set_cfg(&cfg);
	long start = millis();
	try
	{
		if (!SD.begin(SS, SPI, 80000000))
			throw std::runtime_error("SD card failed");

		printThreadingMode();

		initialiseDatabase();

		SQLite::DbConnection commonDb("/sd/test.db");

		std::thread t1(accessDatabaseWrite, 1, std::ref(commonDb));
		std::thread t2(accessDatabaseWrite, 2, std::ref(commonDb));
		std::thread t3(accessDatabaseRead, 3, std::ref(commonDb));

		t1.join();
		t2.join();
		t3.join();

		printTable();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	long end = millis();
	std::cout << "Time taken: " << end - start << "ms\n";
}

// the loop function runs over and over again until power down or reset
void loop()
{

}

void initialiseDatabase()
{
	if (SD.exists("/test.db"))
		SD.remove("/test.db");

	SQLite::DbConnection db("/sd/test.db");
	db.prepare("CREATE TABLE IF NOT EXISTS test (id INTEGER PRIMARY KEY AUTOINCREMENT, phone TEXT)").evaluate();

	SQLite::SQLStatement insertion = db.prepare("INSERT INTO test (phone) VALUES (?)");
	for (int i = 0; i < 10; ++i)
	{
		SQLite::ReusableSQLStatement reusableInsertion(insertion);
		uint8_t buffer[11];
		esp_fill_random(buffer, sizeof(buffer));
		std::string phone = "+";
		for (auto& byte : buffer)
			phone += std::to_string(byte % 10);

		reusableInsertion->bind(phone).evaluate();
	}

	SQLite::SQLStatement selection = db.prepare("SELECT * FROM test");
	int columnCount = selection.getColumnCount();
	std::cout << "Column count: " << columnCount << '\n';
	while (selection.evaluate())
	{
		for (int i = 0; i < columnCount; ++i)
			std::cout << selection.getColumnName(i) << ": " << selection.getColumnValue<std::string>(i) << ", ";
		std::cout << '\n';
	}
}

void accessDatabaseRead(int threadId, SQLite::DbConnection& db)
{
	SQLite::SQLStatement stmt = db.prepare("SELECT * FROM test");
	int columnCount = stmt.getColumnCount();
	std::cout << "Column count: " << columnCount << '\n';
	while (stmt.evaluate())
	{
		for (int i = 0; i < columnCount; ++i)
			std::cout << threadId << " - " << stmt.getColumnName(i) << ": " << stmt.getColumnValue<std::string>(i) << ", ";
		std::cout << '\n';
	}
}

void accessDatabaseWrite(int threadId, SQLite::DbConnection& db)
{
	SQLite::SQLStatement stmt = db.prepare("INSERT INTO test (phone) VALUES (?)");
	for (int i = 0; i < 30; ++i)
	{
		SQLite::ReusableSQLStatement reusableStmt(stmt);
		uint8_t buffer[11];
		esp_fill_random(buffer, sizeof(buffer));
		std::string phone = "+";
		for (auto& byte : buffer)
			phone += std::to_string(byte % 10);

		reusableStmt->bind(phone).evaluate();
		std::cout << threadId << " - Write complete\n";
	}
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

void printThreadingMode()
{
	int mode = sqlite3_threadsafe();
	switch (mode)
	{
	case 0:
		Serial.println("Single-thread");
		break;
	case 1:
		Serial.println("Multi-thread");
		break;
	case 2:
		Serial.println("Serialized");
		break;
	}
}
