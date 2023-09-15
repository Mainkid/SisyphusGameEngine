#include "ErrorLogger.h"
#include <Windows.h>

void ErrorLogger::Log(int errorCode, const std::string& fileName, int lineNum)
{
	std::string message =	"File: " + fileName + "\n" +
							"Line: " + std::to_string(lineNum) + "\n";
	switch (errorCode)
	{
	case ERRORLG_EMPTY:
		break;
	case ERRORLG_GENERIC:
		message = "A non-critical error occured!\n" + message;
		MessageBoxA(NULL, message.c_str(), "Error", MB_ICONERROR);
		break;
	case ERRORLG_GENERIC_CRITICAL :
		message = "A critical error occured!\n" + message;
		MessageBoxA(NULL, message.c_str(), "Error", MB_ICONERROR);
		exit(-1);
		break;
	default:
		break;
	}
}
