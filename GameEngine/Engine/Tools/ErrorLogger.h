#pragma once
#include <string>
#include "P7_Trace.h"
#include "../Core/IService.h"
#include <map>
#include <vector>
#define SY_LOG_ERLOG(logLevel, message) Log(L"ERLOG", logLevel, message)

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

struct SyErrorLogger : public IService
{
	enum Sink
	{
		SY_SINK_CONSOLE
		//...
	};
	enum LogLevel
	{
		SY_LOGLEVEL_DEBUG,
		SY_LOGLEVEL_INFO,
		SY_LOGLEVEL_WARNING,
		SY_LOGLEVEL_ERROR,
		SY_LOGLEVEL_CRITICAL
	};
public:
	//initializes client for default sink and 1 channel for error logger messages
	SyErrorLogger();
	~SyErrorLogger();
	//Add trace channel to all sinks
	SyResult AddChannel(const t_channelName& channelName_);
	//Add sink to all channels
	SyResult AddSink(const t_sinkName& sinkName_, const std::wstring& params);
	//Log message to a channel through all sinks
	SyResult Log(const t_channelName& channelName_, LogLevel logLevel_, const std::wstring& message_);
	//this sink will be passed to the client in error logger constructor
	Sink defaultSink = SY_SINK_CONSOLE;
private:

	std::vector	<t_sinkName>			sinkNames;
	std::vector <t_channelName>			channelNames;
	std::map<t_sinkName, IP7_Client*>	elClients; //map of clients, a client per message sink
	std::map<t_channelName, t_sinkMap>	elTraces; //table of traces, CHANNELSxSINKS
	
};

