#pragma once
#include <string>
#include "../../Tools/ErrorLogger.h"

#define SY_LOG_EVSY(logLevel_, ...) SY_LOG("EVSY",	logLevel_, __VA_ARGS__)

struct SyThisFrameEventTag {
};

struct SyEventTag
{
};

struct SyTestEvent
{
    std::string message;
};