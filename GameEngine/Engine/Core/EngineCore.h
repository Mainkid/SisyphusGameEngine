#pragma once

#include <chrono>
#include <windows.h>
#include <WinUser.h>
#include <wrl.h>
#include <iostream>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <vector>

#include "ServiceLocator.h"
#include "../Core/DisplayWin32.h"
#include "../Core/HUD/Hud.h"
#include "../Systems/Systems.h"
#include "../Core/Rendering/RenderTarget.h"
#include "../Systems/EngineContext.h"
#include "Scene/Scene.h"


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

using namespace Microsoft::WRL;


class EngineCore
{
public:
    std::chrono::time_point<std::chrono::steady_clock> PrevTime;
    std::unique_ptr<Hud> hud;
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