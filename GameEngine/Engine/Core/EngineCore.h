#pragma once

#include <chrono>
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <wrl.h>
#include <vector>

#include "ServiceLocator.h"
#include "../Systems/Systems.h"
#include "../Systems/EngineContext.h"
#include "../Systems/ResourceService.h"


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

using namespace Microsoft::WRL;


class EngineCore
{
public:
    std::chrono::time_point<std::chrono::steady_clock> PrevTime;
    std::vector<std::unique_ptr<ISystem>> systems;
    EngineContext* ec;

    static EngineCore* instance()
    {
        static EngineCore inst;
        return &inst;
    }

    EngineCore();
    ~EngineCore() {};

    HWND GetWindowHWND();
    void StartUp();
    void StartUpSystems();
    void ShutDown();
    void StartUpdateLoop();

protected:
    void Render();
    void Update();

    
};