#pragma once
#include "../../ISystem.h"
#include <filesystem>
#include <fstream>
#include "../RenderContext.h"
#include "../../EngineContext.h"
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

    std::vector<xstring> messages;
    std::map<char, bool> filters; //if filters['i']==true, info messages are show and so on
};

