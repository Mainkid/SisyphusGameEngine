#pragma once
#include <string>
#include "P7_Trace.h"
#include "../Core/IService.h"
#include <map>
#include <vector>
#include "../Core/ServiceLocator.h"
#include <iostream>

#pragma region Log_macro
#define SY_EL ((ServiceLocator::instance() == nullptr) ? nullptr : ServiceLocator::instance()->Get<SyErrorLogger>())		\

#define SY_LOG(channelName_, logLevel_, ...)								\
if (SY_EL != nullptr)														\
{																			\
	std::vector<IP7_Trace*> traces = SY_EL->GetTraces(channelName_);		\
	for (auto& trace : traces)												\
		trace->Trace(	0,													\
						SY_EL->GetP7TraceLevel(logLevel_),					\
						nullptr,											\
						(tUINT16)__LINE__,									\
						__FILE__,											\
						__FUNCTION__,										\
						__VA_ARGS__);										\
}																			\
else																		\
	std::cout << "FATAL:  ServiceLocator::instance() or ServiceLocator::instance()->Get<SyErrorLogger>() is nullptr\n"\

#pragma endregion

#define SY_LOG_ERLOG(logLevel, ...)	SY_LOG(L"ERLOG",	logLevel, __VA_ARGS__)
#define SY_LOG_PHYS(logLevel, ...)	SY_LOG(L"PHYS",		logLevel, __VA_ARGS__)
#define SY_LOG_CORE(logLevel, ...)	SY_LOG(L"CORE",		logLevel, __VA_ARGS__)
#define SY_LOG_REND(logLevel, ...)	SY_LOG(L"REND",		logLevel, __VA_ARGS__)
#define SY_LOG_HUD(logLevel, ...)	SY_LOG(L"HUD",		logLevel, __VA_ARGS__)

#define SY_RESCODE_OK 0
#define SY_RESCODE_UNEXPECTED 1
#define SY_RESCODE_ERROR 2

//basic structure used for return values
struct SyResult
{
	int code = SY_RESCODE_OK;
	std::wstring message = L"OK";
};

typedef std::wstring t_sinkName;
typedef std::wstring t_channelName;
typedef std::map<t_sinkName, IP7_Trace*> t_sinkMap;

enum SyElSink
{
	SY_SINK_CONSOLE,
	SY_SINK_TXT
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
public:
	//initializes client for default sink and 1 channel for error logger messages
	SyErrorLogger();
	~SyErrorLogger();
	//Add trace channel to all sinks
	SyResult AddChannel(const t_channelName& channelName_);
	//Add sink to all channels
	SyResult AddSink(const t_sinkName& sinkName_, const std::wstring& params);
	//Returns vector of traces by the channel and all sinks
	std::vector<IP7_Trace*> GetTraces(const t_channelName& channelName_);
	//Converts SyElLogLevel object to eP7Trace_Level object
	eP7Trace_Level GetP7TraceLevel(SyElLogLevel logLevel);
	//this sink will be passed to the client in error logger constructor
	SyElSink defaultSink = SY_SINK_TXT;
	std::wstring defaultMessageFormat = L"%cn:\t[%ix]\t(%tm)\t%lv\t{%fs, %fl, %fn} : %ms";
	std::wstring logFileDir = L"logs";
private:

	std::vector	<t_sinkName>			sinkNames;
	std::vector <t_channelName>			channelNames;
	std::map<t_sinkName, IP7_Client*>	elClients; //map of clients, a client per message sink
	std::map<t_channelName, t_sinkMap>	elTraces; //table of traces, CHANNELSxSINKS
	
};

