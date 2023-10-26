#include "ErrorLoggingSystem.h"
#include <filesystem>
SyResult SyErrorLoggingSystem::Init()
{
	if (std::find(SY_EL->sinks.begin(), SY_EL->sinks.end(), SY_SINK_TXT) != SY_EL->sinks.end())
	{
		using namespace std::chrono;
		std::time_t tt;
		auto now = system_clock::now();
		tt = system_clock::to_time_t(now);
		tm timeInfo;
		localtime_s(&timeInfo, &tt);
		logPath = xstring(	"%s\\%s%d-%s%d-%s%d_%s%d-%s%d-%s%d-%s%d.txt",
							logDir.c_str(),
							GetZeros(3, timeInfo.tm_year - 100).c_str(),
							timeInfo.tm_year - 100,
							GetZeros(2, timeInfo.tm_mon + 1).c_str(),
							timeInfo.tm_mon + 1,
							GetZeros(2, timeInfo.tm_mday).c_str(),
							timeInfo.tm_mday,
							GetZeros(2, timeInfo.tm_hour).c_str(),
							timeInfo.tm_hour,
							GetZeros(2, timeInfo.tm_min).c_str(),
							timeInfo.tm_min,
							GetZeros(2, timeInfo.tm_sec).c_str(),
							timeInfo.tm_sec,
							GetZeros(3, int(now.time_since_epoch().count() / 1000000 % 1000)).c_str(),
							int(now.time_since_epoch().count() / 1000000 % 1000)).ToString();
		fout = std::ofstream(logPath);
	}
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "Error logging system initialization successful. ");
	return SyResult();
}

SyResult SyErrorLoggingSystem::Run()
{
	std::filesystem::path logPath_(logPath);
	unsigned fileCtr = 0;
	for (auto& directoryEntry : std::filesystem::directory_iterator(logPath_))
	{
		fileCtr++;
		if (fileCtr > 10)
		{
			std::filesystem::remove(directoryEntry);
			directoryEntry = directoryEntry.
		}
	}
	for (auto& sink : SY_EL->sinks)
		for (auto& message : SY_EL->messagePool)
		{
			switch (sink)
			{
			case SY_SINK_CONSOLE:
				std::cout << message.ToString();
				break;
			case SY_SINK_TXT:
				fout << message.ToString();
				break;
			default:
				break;
			}
		}
	SY_EL->messagePool.clear(); //all systems accessing message pool must do it prior to SyErrorLoggingSystem::Run()!

	
	return SyResult();
}

SyResult SyErrorLoggingSystem::Destroy()
{
	Run();
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "Error logging system destruction successful. ");
	return SyResult();
}

std::string SyErrorLoggingSystem::GetZeros(const unsigned maxZeros_, unsigned number_)
{
	unsigned numDigits = 1;
	if (maxZeros_ > 3)
		std::cout << "";
	if (number_ >= 10)
		numDigits++;
	if (number_ >=100)
		numDigits++;
	if (number_ >= 1000)
		std::cout << "";
	unsigned numZeros = maxZeros_ - numDigits;
	switch (numZeros)
	{
	case 0:
		return "";
	case 1:
		return "0";
	case 2:
		return "00";
	default:
		std::cout << "";
		return "";
	}
}
