#include "ErrorLoggingSystem.h"
#include <filesystem>

unsigned SyErrorLoggingSystem::logDirCapacity = 10;

void SyErrorLoggingSystem::SetLogDirectoryCapacity(unsigned newCapacity_)
{
	logDirCapacity = newCapacity_;
}

unsigned SyErrorLoggingSystem::GetLogDirectoryCapacity()
{
	return logDirCapacity;
}

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

	std::filesystem::path logDirPath(logDir);
	unsigned fileCtr = 0;
	std::vector<std::filesystem::path> logFiles;
	logFiles.reserve(2 * logDirCapacity);
	for (auto& logDirEntry : std::filesystem::directory_iterator(logDirPath))
		logFiles.push_back(logDirEntry.path());
	for (auto i = 0; i < (int)logFiles.size() - (int)logDirCapacity; i++)
			std::filesystem::remove(logFiles[i]);

	return SyResult();
}

SyResult SyErrorLoggingSystem::Run()
{
	
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
	return SyResult();
}

std::string SyErrorLoggingSystem::GetZeros(const unsigned maxZeros_, unsigned number_)
{
	unsigned numDigits = 1;
	if (number_ >= 10)
		numDigits++;
	if (number_ >=100)
		numDigits++;
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
		return "";
	}
}
