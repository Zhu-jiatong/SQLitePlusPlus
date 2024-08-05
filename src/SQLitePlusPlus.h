/*
 Name:		SQLitePlusPlus.h
 Created:	5/10/2024 12:14:45 AM
 Author:	zhuji
 Editor:	http://www.visualmicro.com
*/

#ifndef _SQLitePlusPlus_h
#define _SQLitePlusPlus_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "DbConnection.h"
#include "SQLStatement.h"
#include "SQLValue.h"

namespace SQLite {
	void initialise();
	void shutdown();
}

#endif

