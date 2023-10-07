#include "ErrorLoggingSystem.h"

SyResult SyErrorLoggingSystem::Init()
{
	if (std::find(SY_EL->sinks.begin(), SY_EL->sinks.end(), SY_SINK_TXT) != SY_EL->sinks.end())
	{
		using namespace std::chrono;
		std::time_t tt;
		auto now = system_clock::now();
		tt = system_clock::to_time_t(now);
		tm timeinfo;
		localtime_s(&timeinfo, &tt);
		logPath = xstring(	"logs\\%d-%d-%d-%d.txt",
							timeinfo.tm_hour,
							timeinfo.tm_min,
							timeinfo.tm_sec,
							int(now.time_since_epoch().count() / 1000000 % 1000)).ToString();
		fout = std::ofstream(logPath);
	}
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "Error logging system initialization successful. ");
	return SyResult();
}

SyResult SyErrorLoggingSystem::Run()
{
	using namespace std::chrono;
	std::time_t tt;
	auto now = system_clock::now();
	tt = system_clock::to_time_t(now);
	tm timeinfo;
	localtime_s(&timeinfo, &tt);
	for (auto& sink : SY_EL->sinks)
		for (auto& message : SY_EL->messagePool)
		{
			switch (sink)
			{
			case SY_SINK_CONSOLE:
				std::cout << message;
				break;
			case SY_SINK_TXT:
				fout << message;
				break;
			default:
				break;
			}
		}
	SY_EL->messagePool.clear(); //all systems accessing messagepool must do it prior to SyErrorLoggingSystem::Run()!
	return SyResult();
}

SyResult SyErrorLoggingSystem::Destroy()
{
	Run();
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "Error logging system destruction successful. ");
	return SyResult();
}
