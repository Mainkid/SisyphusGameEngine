#pragma once
#include "../Tools/ErrorLogger.h"

#define SY_LOG_EVSY(logLevel, ...)	SY_LOG("EVSY",	logLevel, __VA_ARGS__)

struct SyEventBase
{
    SyEventBase() = default;
    virtual ~SyEventBase() {};
protected:
    std::string name; //must be initialized within Event struct definition
private:
    bool isReservedForNextFrame = true;

    template<typename T_System>
    friend class SystemBase;
};

struct SyTestEvent : public SyEventBase
{
    SyTestEvent() = default;
    SyTestEvent(const std::string& message_)
    {
        message = message_;
        name = "Test";
    }
    std::string message;
};