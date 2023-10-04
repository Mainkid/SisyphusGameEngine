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
#include "ECS/SystemsGroup.h"


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

using namespace Microsoft::WRL;


class EngineCore
{
public:
    std::chrono::time_point<std::chrono::steady_clock> PrevTime;

    static EngineCore* instance()
    {
        static EngineCore inst;
        return &inst;
    }

    EngineCore();
    ~EngineCore() {};

    HWND GetWindowHWND();
    void StartUp();
    void ShutDown();
    void Update();

protected:
    void UpdateImpl();

private:
    SystemsGroup _systems;

    EngineContext* _context;

    void StartUpSystems();
};