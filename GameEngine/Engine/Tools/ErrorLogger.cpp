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
		clientParams = L"/P7.Sink=Console /P7.Format=\"%cn:\t[%ix]\t(%tm)\t%lv\t{%fs, %fl, %fn} : %ms\"";
		//clientParams = L"/P7.Sink=Console /P7.Format=\"%cn:\t[%ix]\t(%tm)\t%lv\t\%tn\t{File: %fs, Line: %fl, Function: %fn} : %ms\"";
		break;
	//case...
	}
	//P7_Set_Crash_Handler();
	channelNames.push_back(L"ERLOG");
	IP7_Client* client = P7_Create_Client(clientParams.c_str());
	if (client == nullptr)
	{
		std::cout << "Failed to complete SyErrorLogger constructor.\n";
		return;
	}
	elClients.insert({L"Console", client});
	auto& newSinkMap = elTraces.insert({ L"ERLOG", t_sinkMap() }).first->second;
	IP7_Trace* trace = P7_Create_Trace(elClients.at(L"Console"), L"ERLOG");
	if (trace == nullptr)
	{
		std::cout << "Failed to complete SyErrorLogger constructor.\n";
		return;
	}
	newSinkMap.insert({L"Console", trace});
	SyResult result = AddChannel(L"PHYS");
	if (result.code == SY_RESCODE_ERROR)
	{
		Log(L"ERLOG", SY_LOGLEVEL_ERROR, L"Failed to complete SyErrorLogger constructor.");
		return;
	}
	result = AddChannel(L"CORE");
	if (result.code == SY_RESCODE_ERROR)
	{
		Log(L"ERLOG", SY_LOGLEVEL_ERROR, L"Failed to complete SyErrorLogger constructor.");
		return;
	}
	result = AddChannel(L"REND");
	if (result.code == SY_RESCODE_ERROR)
	{
		Log(L"ERLOG", SY_LOGLEVEL_ERROR, L"Failed to complete SyErrorLogger constructor.");
		return;
	}
	result = AddChannel(L"HUD");
	if (result.code == SY_RESCODE_ERROR)
	{
		Log(L"ERLOG", SY_LOGLEVEL_ERROR, L"Failed to complete SyErrorLogger constructor.");
		return;
	}
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
			result.message = L"Channel " + channelName_ + L" already exists.";
			Log(L"ERLOG", SY_LOGLEVEL_WARNING, result.message);
			return result;
		}
	channelNames.push_back(channelName_);
	auto& newSinkMap = elTraces.insert({ channelName_, t_sinkMap() }).first->second;
	for (auto& sinkName : sinkNames)
	{
		IP7_Client* client = elClients.find(sinkName)->second;
		if (client == nullptr)
		{
			Log(L"ERLOG", SY_LOGLEVEL_ERROR, L"client == nullptr");
			result.code = SY_RESCODE_ERROR;
			result.message = L"Failed to create new IP7_Trace";
			return result;
		}
		IP7_Trace* newTrace = P7_Create_Trace(client, channelName_.c_str());
		if (client == nullptr)
		{
			Log(L"ERLOG", SY_LOGLEVEL_ERROR, L"newTrace == nullptr");
			result.code = SY_RESCODE_ERROR;
			result.message = L"Failed to create new IP7_Trace";
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
			Log(L"ERLOG", SY_LOGLEVEL_WARNING, result.message);
			return result;
		}
	IP7_Client* newClient = P7_Create_Client(params.c_str());
	if (newClient == nullptr)
	{
		SY_LOG_ERLOG(SY_LOGLEVEL_ERROR, L"newClient == nulptr");
		result.code = SY_RESCODE_ERROR;
		result.message = L"Failed to create new client.";
		return result;
	}
	elClients.insert({ sinkName_,  newClient});
	for (auto& channelSinkMapPair : elTraces)
	{
		auto& sinkMap = channelSinkMapPair.second;
		auto& channelName = channelSinkMapPair.first;
		IP7_Trace* newTrace = P7_Create_Trace(newClient, channelName.c_str());
		if (newTrace == nullptr)
		{
			Log(L"ERLOG", SY_LOGLEVEL_ERROR, L"newTrace == nulptr");
			result.code = SY_RESCODE_ERROR;
			result.message = L"Failed to create new trace.";
			return result;
		}
		sinkMap.insert({sinkName_, newTrace});
	}
	return result;
}

SyResult SyErrorLogger::Log(const t_channelName& channelName_, SyElLogLevel traceLevel_, const std::wstring& message_)
{
	SyResult result;
	auto search = elTraces.find(channelName_);
	if (search == elTraces.end())
	{
		result.code = SY_RESCODE_UNEXPECTED;
		result.message = channelName_ + L" is an unknown channel name.";
		Log(L"ERLOG", SY_LOGLEVEL_ERROR, result.message);
		return result;
	}

	auto tableLine = search->second;
	for (auto& tracePair : tableLine)
	{
		IP7_Trace* trace = tracePair.second;
		switch (traceLevel_)
		{
		case SY_LOGLEVEL_DEBUG:
			trace->P7_DEBUG(0, message_.c_str());
			break;
		case SY_LOGLEVEL_INFO:
			trace->P7_INFO(0, message_.c_str());
			break;
		case SY_LOGLEVEL_WARNING:
			trace->P7_WARNING(0, message_.c_str());
			break;
		case SY_LOGLEVEL_ERROR:
			trace->P7_ERROR(0, message_.c_str());
			break;
		case SY_LOGLEVEL_CRITICAL:
			trace->P7_CRITICAL(0, message_.c_str());
			break;
		default:
			result.code = SY_RESCODE_UNEXPECTED;
			result.message = L" Unknown channel name.";
			Log(L"ERLOG", SY_LOGLEVEL_ERROR, result.message);
			return result;
			break;
		}
	}
	return result;
}



//void ErrorLogger::Log(int errorCode, const std::string& fileName, int lineNum)
//{
//	std::string message =	"File: " + fileName + "\n" +
//							"Line: " + std::to_string(lineNum) + "\n";
//	switch (errorCode)
//	{
//	case SY_NO_ERROR:
//		break;
//	case SY_GENERIC_ERROR:
//		message = "A non-critical error occured!\n" + message;
//		MessageBoxA(NULL, message.c_str(), "Error", MB_ICONERROR);
//		break;
//	case SY_GENERIC_ERROR_CRITICAL :
//		message = "A critical error occured!\n" + message;
//		MessageBoxA(NULL, message.c_str(), "Error", MB_ICONERROR);
//		exit(-1);
//		break;
//	default:
//		break;
//	}
//}
