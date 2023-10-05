#include "ErrorLogger.h"
#include <Windows.h>
#include <iostream>



SyErrorLogger::SyErrorLogger()
{
	std::wstring clientParams;
	switch (defaultSink)
	{
	case(SY_SINK_CONSOLE):
		sinkNames.push_back(L"Console");
		clientParams = L"/P7.Sink=Console /P7.Format=" + defaultMessageFormat;
		break;
	case(SY_SINK_TXT):
		sinkNames.push_back(L"FileTxt");
		clientParams =	L"/P7.Sink=FileTxt /P7.Format=" + defaultMessageFormat + L" /P7.Dir=" + logFileDir + L"\\";
		break;
	//case...
	}
	P7_Set_Crash_Handler();
	channelNames.push_back(L"ERLOG");
	IP7_Client* client = P7_Create_Client(clientParams.c_str());
	if (client == nullptr)
	{
		std::cout << "Failed to complete SyErrorLogger constructor.\n";
		return;
	}
	elClients.insert({sinkNames[0], client});
	auto& newSinkMap = elTraces.insert({ L"ERLOG", t_sinkMap() }).first->second;
	IP7_Trace* trace = P7_Create_Trace(elClients.at(sinkNames[0]), L"ERLOG");
	if (trace == nullptr)
	{
		std::cout << "Failed to complete SyErrorLogger constructor.\n";
		return;
	}
	newSinkMap.insert({ sinkNames[0], trace});
	AddChannel(L"PHYS");
	AddChannel(L"CORE");
	AddChannel(L"REND");
	AddChannel(L"HUD");
}

SyErrorLogger::~SyErrorLogger()
{
	for (auto& client : elClients)
		client.second->Release();
	for (auto& channelAndSinkMapPair : elTraces)
		for (auto& tracePair : channelAndSinkMapPair.second)
			tracePair.second->Release();
}

SyResult SyErrorLogger::AddChannel(const t_channelName& channelName_)
{
	SyResult result;
	for (auto& channelName : channelNames)
		if (channelName == channelName_)
		{
			result.code = SY_RESCODE_UNEXPECTED;
			result.message = L"Channel already exists. ";
			SY_LOG_ERLOG(SY_LOGLEVEL_WARNING, L"Trace with channel %%s already exists. ", channelName_.c_str());
			return result;
		}
	channelNames.push_back(channelName_);
	auto& newSinkMap = elTraces.insert({ channelName_, t_sinkMap() }).first->second;
	for (auto& sinkName : sinkNames)
	{
		IP7_Client* client = elClients.find(sinkName)->second;
		if (client == nullptr)
		{
			result.code = SY_RESCODE_ERROR;
			result.message = L"Failed to create new IP7_Trace";
			SY_LOG_ERLOG(SY_LOGLEVEL_ERROR, L"client == nullptr");
			return result;
		}
		IP7_Trace* newTrace = P7_Create_Trace(client, channelName_.c_str());
		if (newTrace == nullptr)
		{
			result.code = SY_RESCODE_ERROR;
			result.message = L"Failed to create new IP7_Trace";
			SY_LOG_ERLOG(SY_LOGLEVEL_ERROR, L"newTrace == nullptr");
			return result;
		}
		newSinkMap.insert({ sinkName,  newTrace});
	}
	return result;
}

SyResult SyErrorLogger::AddSink(const t_sinkName& sinkName_, const std::wstring& params)
{
	SyResult result;
	for (auto& sinkName : sinkNames)
		if (sinkName == sinkName_)
		{
			result.code = SY_RESCODE_UNEXPECTED;
			result.message = L"Sink " + sinkName_ + L" already exists.";
			SY_LOG_ERLOG(SY_LOGLEVEL_WARNING, L"Client with %%s sink already exists. ", sinkName_);
			return result;
		}
	IP7_Client* newClient = P7_Create_Client(params.c_str());
	if (newClient == nullptr)
	{
		result.code = SY_RESCODE_ERROR;
		result.message = L"Failed to create new client.";
		SY_LOG_ERLOG(SY_LOGLEVEL_ERROR, L"Failed to create new client. Possible params wrong formatting.");
		return result;
	}
	elClients.insert({ sinkName_,  newClient});
	for (auto& channelSinkMapPair : elTraces)
	{
		auto& sinkMap = channelSinkMapPair.second;
		auto& channelName = channelSinkMapPair.first;
		IP7_Trace* newTrace = P7_Create_Trace(newClient, channelName.c_str());
		sinkMap.insert({sinkName_, newTrace});
	}
	return result;
}


std::vector<IP7_Trace*> SyErrorLogger::GetTraces(const t_channelName& channelName_)
{
	std::vector<IP7_Trace*> traces;
	auto search = elTraces.find(channelName_);
	if (search == elTraces.end())
	{
		SY_LOG_ERLOG(SY_LOGLEVEL_ERROR, L"%%s is an unknown channel name.", channelName_.c_str());
		return traces;
	}
	auto& tableLine = search->second;
	for (auto& tracePair : tableLine)
		traces.push_back(tracePair.second);
	return traces;
}

eP7Trace_Level SyErrorLogger::GetP7TraceLevel(SyElLogLevel logLevel)
{
	switch (logLevel)
	{
	case SY_LOGLEVEL_DEBUG:
		return eP7Trace_Level::EP7TRACE_LEVEL_DEBUG;
		break;
	case SY_LOGLEVEL_INFO:
		return eP7Trace_Level::EP7TRACE_LEVEL_INFO;
		break;
	case SY_LOGLEVEL_WARNING:
		return eP7Trace_Level::EP7TRACE_LEVEL_WARNING;
		break;
	case SY_LOGLEVEL_ERROR:
		return eP7Trace_Level::EP7TRACE_LEVEL_ERROR;
		break;
	case SY_LOGLEVEL_CRITICAL:
		return eP7Trace_Level::EP7TRACE_LEVEL_CRITICAL;
		break;
	}
	return eP7Trace_Level();
}



