#include "ErrorLogger.h"
#include <Windows.h>

void ErrorLogger::Log(int errorCode, const std::string& fileName, int lineNum)
{
	std::string message =	"File: " + fileName + "\n" +
							"Line: " + std::to_string(lineNum) + "\n";
	switch (errorCode)
	{
	case SY_NO_ERROR:
		break;
	case SY_GENERIC_ERROR:
		message = "A non-critical error occured!\n" + message;
		MessageBoxA(NULL, message.c_str(), "Error", MB_ICONERROR);
		break;
	case SY_GENERIC_ERROR_CRITICAL :
		message = "A critical error occured!\n" + message;
		MessageBoxA(NULL, message.c_str(), "Error", MB_ICONERROR);
		exit(-1);
		break;
	default:
		break;
	}
}
