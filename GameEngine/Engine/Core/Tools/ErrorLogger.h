#pragma once
#include <string>
#include "ComplexString.h"
#include "../IService.h"
#include <vector>
#include "../ServiceLocator.h"
#include <iostream>
#include <chrono>
#pragma region Log_macro
#define SY_EL ((ServiceLocator::instance() == nullptr) ? nullptr : ServiceLocator::instance()->Get<SyErrorLogger>())		\

#define SY_LOG(channelName_, logLevel_, ...)											\
if (SY_EL != nullptr)																	\
{																						\
	SY_EL->Log(channelName_, logLevel_, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);	\
}																						\
else																					\
	std::cout << "FATAL:  ServiceLocator::instance() or ServiceLocator::instance()->Get<SyErrorLogger>() is nullptr\n"\

#pragma endregion

#define SY_LOG_ERLG(logLevel, ...)	SY_LOG("ERLG",	logLevel, __VA_ARGS__)
#define SY_LOG_PHYS(logLevel, ...)	SY_LOG("PHYS",	logLevel, __VA_ARGS__)
#define SY_LOG_AUDI(logLevel, ...)	SY_LOG("AUDI",	logLevel, __VA_ARGS__)
#define SY_LOG_CORE(logLevel, ...)	SY_LOG("CORE",	logLevel, __VA_ARGS__)
#define SY_LOG_REND(logLevel, ...)	SY_LOG("REND",	logLevel, __VA_ARGS__)
#define SY_LOG_HUD(logLevel, ...)	SY_LOG("HUD ",	logLevel, __VA_ARGS__)
#define SY_LOG_EVSY(logLevel_, ...) SY_LOG("EVSY",	logLevel_, __VA_ARGS__)
#define SY_LOG_MONO(logLevel, ...)  SY_LOG("MONO",  logLevel, __VA_ARGS__)
#define SY_LOG_GAME(logLevel, ...)	SY_LOG("GAME",	logLevel, __VA_ARGS__)

#define SY_RESCODE_OK 0
#define SY_RESCODE_UNEXPECTED 1
#define SY_RESCODE_ERROR 2

#define SY_RESULT_CHECK(result) if ((result).code != SY_RESCODE_OK) return result
#define SY_RESULT_CHECK_EXT(result, prefix)												\
	if ((result).code != SY_RESCODE_OK)													\
		return SyResult{																\
			(result).code,																\
			SyComplexString(SyComplexString(prefix).ToString() + std::string("; %s"),	\
											(result).message.ToString().c_str())}

//basic structure used for return values
struct SyResult
{
	int code = SY_RESCODE_OK;
	xstring message = "OK";
};

enum SyElSink
{
	SY_SINK_CONSOLE,
	SY_SINK_TXT,
	SY_SINK_UI
	//...
};
enum SyElLogLevel
{
	SY_LOGLEVEL_DEBUG,
	SY_LOGLEVEL_INFO,
	SY_LOGLEVEL_WARNING,
	SY_LOGLEVEL_ERROR,
	SY_LOGLEVEL_CRITICAL
};

struct SyErrorLogger : IService
{
	SyErrorLogger()
	{
		logLevelMap = { { SY_LOGLEVEL_DEBUG,	"DEBUG"},
						{ SY_LOGLEVEL_INFO,		"INFO " },
						{ SY_LOGLEVEL_WARNING,	"WARN " },
						{ SY_LOGLEVEL_ERROR,	"ERROR" },
						{ SY_LOGLEVEL_CRITICAL, "CRIT " } };
		channelNameMap = {	{"CORE", 0},
							{"REND", 0}, 
							{"PHYS", 0}, 
							{"HUD ", 0}, 
							{"ERLG", 0},
							{"MONO", 0},
							{"GAME", 0}};
		sinks = { /*SY_SINK_CONSOLE,*/ SY_SINK_TXT };
	};

	template <typename ...Args>
	SyResult Log(const std::string& channelName_, SyElLogLevel logLevel_, const std::string& filePath_, const std::string& funName_, int line_, std::string message_, Args... args_)
	{
		SyResult result;
		auto fileNamePos = filePath_.find_last_of('\\') + 1;
		auto fileName = filePath_.substr(fileNamePos);
		if (channelNameMap.contains(channelName_))
		{
			std::string smessage = xstring(message_, args_...).ToString();
			using namespace std::chrono;
			std::time_t tt;
			auto now = system_clock::now();
			tt = system_clock::to_time_t(now);
			tm timeinfo;
			localtime_s(&timeinfo, &tt);
			//if (logLevel_ != SY_LOGLEVEL_INFO)
			//	std::cout << "";
			xstring newMessage(	"#%d\t%s\t[%s]\t|%d : %d : %d : %d|\t{%s, %s, %d} : %s\n",
								counter,
								logLevelMap.at(logLevel_).c_str(),
								channelName_.c_str(),
								timeinfo.tm_hour,
								timeinfo.tm_min,
								timeinfo.tm_sec,
								int(now.time_since_epoch().count() / 1000000 % 1000),
								fileName.c_str(),
								funName_.c_str(),
								line_,
								smessage.c_str());
			messagePool.push_back(newMessage);
			counter++;
		}
		else
		{
			result.code = SY_RESCODE_ERROR;
			result.message = xstring("Channel %s is not registered. ", channelName_.c_str());
			SY_LOG_ERLG(SY_LOGLEVEL_ERROR, "Channel %s is not registered in error logger. ", channelName_.c_str());
		}
		return result;
	}
	SyResult RegisterChannel(const std::string& channelName_)
	{
		SyResult result;
		if (channelNameMap.contains(channelName_))
		{
			result.code = SY_RESCODE_UNEXPECTED;
			result.message = xstring("Logger already has channel %s", channelName_.c_str());
		}
		else
			channelNameMap.insert({channelName_, 0});
		return result;
	}
private:
	std::vector<xstring> messagePool;
	std::vector<SyElSink> sinks;
	std::unordered_map<SyElLogLevel, std::string> logLevelMap;
	std::unordered_map<std::string, int> channelNameMap;
	int counter = 0;

	friend class SyErrorLoggingSystem;
	friend class SyHudConsoleSystem;
};


