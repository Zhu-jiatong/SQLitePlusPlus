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

std::shared_ptr<DbConnection> commonDb;

// the setup function runs once when you press reset or power the board
void setup()
{
	Serial.begin(115200);

	esp_pthread_cfg_t cfg = esp_pthread_get_default_config();
	cfg.stack_size = 4096 * 2;
	esp_pthread_set_cfg(&cfg);

	try
	{
		if (!SD.begin())
			throw std::runtime_error("SD card failed");

		initialiseDatabase();

		commonDb = std::make_shared<DbConnection>("/sd/test.db");

		std::thread t1(accessDatabaseWrite, 1, commonDb);
		std::thread t2(accessDatabaseWrite, 2, commonDb);
		std::thread t3(accessDatabaseRead, 3, commonDb);

		t1.join();
		t2.join();
		t3.join();

		printTable();
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

void initialiseDatabase()
{
	if (SD.exists("/test.db"))
		SD.remove("/test.db");

	DbConnection db("/sd/test.db");
	db.prepare("CREATE TABLE IF NOT EXISTS test (id INTEGER PRIMARY KEY AUTOINCREMENT, phone TEXT)").evaluate();

	SQLStatement insertion = db.prepare("INSERT INTO test (phone) VALUES (?)");
	for (int i = 0; i < 10; ++i)
	{
		uint8_t buffer[11];
		esp_fill_random(buffer, sizeof(buffer));
		std::string phone = "+";
		for (auto& byte : buffer)
			phone += std::to_string(byte % 10);

		insertion.bind<>(phone).evaluate();
		insertion.reset();
	}

	SQLStatement selection = db.prepare("SELECT * FROM test");
	int columnCount = selection.getColumnCount();
	std::cout << "Column count: " << columnCount << '\n';
	while (selection.evaluate())
	{
		for (int i = 0; i < columnCount; ++i)
			std::cout << selection.getColumnName(i) << ": " << selection.getColumnValue<std::string>(i) << ", ";
		std::cout << '\n';
	}
}

void accessDatabaseRead(int threadId, std::shared_ptr<DbConnection> db)
{
	SQLStatement stmt = db->prepare("SELECT * FROM test");
	int columnCount = stmt.getColumnCount();
	std::cout << "Column count: " << columnCount << '\n';
	while (stmt.evaluate())
	{
		for (int i = 0; i < columnCount; ++i)
			std::cout << threadId << " - " << stmt.getColumnName(i) << ": " << stmt.getColumnValue<std::string>(i) << ", ";
		std::cout << '\n';
	}
}

void accessDatabaseWrite(int threadId, std::shared_ptr<DbConnection>db)
{
	SQLStatement stmt = db->prepare("INSERT INTO test (phone) VALUES (?)");
	for (int i = 0; i < 30; ++i)
	{
		uint8_t buffer[11];
		esp_fill_random(buffer, sizeof(buffer));
		std::string phone = "+";
		for (auto& byte : buffer)
			phone += std::to_string(byte % 10);

		stmt.bind<>(phone).evaluate();
		stmt.reset();
		std::cout << threadId << " - Write complete\n";
	}
}

void printTable()
{
	DbConnection db("/sd/test.db");
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