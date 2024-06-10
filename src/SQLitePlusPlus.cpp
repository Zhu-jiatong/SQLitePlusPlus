/*
 Name:		SQLitePlusPlus.cpp
 Created:	5/10/2024 12:14:45 AM
 Author:	zhuji
 Editor:	http://www.visualmicro.com
*/

#include "SQLitePlusPlus.h"
#include "SQLiteError.h"

void SQLite::initialise()
{
	int rc = sqlite3_initialize();
	SQLiteError::checkError(rc);
}

void SQLite::shutdown()
{
	int rc = sqlite3_shutdown();
	SQLiteError::checkError(rc);
}
