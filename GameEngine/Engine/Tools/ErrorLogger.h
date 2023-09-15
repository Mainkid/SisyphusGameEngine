#pragma once
#include <string>
#define ERRORLG_EMPTY 0
#define ERRORLG_GENERIC 1
#define ERRORLG_GENERIC_CRITICAL 2
class ErrorLogger
{
public:
	static void Log(int errorCode, const std::string& fileName, int lineNum);
};

