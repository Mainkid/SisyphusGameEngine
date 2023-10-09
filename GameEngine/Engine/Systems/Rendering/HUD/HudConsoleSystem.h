#pragma once
#include "../../ISystem.h"
#include <filesystem>
#include <fstream>
#include "../RenderContext.h"
#include "../../EngineContext.h"
#include "../../../Tools/Data/RingBuffer.h"
class SyHudConsoleSystem : public ISystem
{
public:
    SyResult Init() override;
    SyResult Run() override;
    SyResult Destroy() override;

private:
    EngineContext* ec;
    RenderContext* rc;
    HardwareContext* hc;


    const int MAX_MESSAGES = 1000;
    SyRingBuffer<xstring> messageBuffer = SyRingBuffer<xstring>(MAX_MESSAGES);
    std::map<char, bool> filters; //if filters['i']==true, info messages are shown and so on
};

