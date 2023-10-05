#pragma once
#include "../../ISystem.h"
#include <filesystem>
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

    std::vector<std::string> messages;
    std::filesystem::path logDir = "P7logs";
};

