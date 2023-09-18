#pragma once
#include <string>
#define SY_NO_ERROR 0
#define SY_GENERIC_ERROR 1
#define SY_GENERIC_ERROR_CRITICAL 2
class ErrorLogger
{
public:
	static void Log(int errorCode, const std::string& fileName, int lineNum);
};

