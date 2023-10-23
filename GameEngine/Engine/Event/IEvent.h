#pragma once

struct IEvent
{
    virtual ~IEvent() {};
    
    bool isReservedForNextFrame = true;
    std::string name;
};