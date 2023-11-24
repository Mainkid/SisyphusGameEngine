#pragma once
#include "../../Core/ECS/SystemBase.h"
#include <filesystem>
#include <fstream>
#include "../../../Contexts/RenderContext.h"
#include "../../../Contexts/EngineContext.h"
#include "../../../Contexts/HardwareContext.h"
#include "../../../Core/Tools/Structures/RingBuffer.h"

class SyHudConsoleSystem : public SystemBase
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

