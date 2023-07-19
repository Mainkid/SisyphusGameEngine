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

#include "../Components/GameComponent.h"
#include "../Components/GameObject.h"
#include "../Core/DisplayWin32.h"
#include "../Core/WinInput.h"
#include "../Core/Rendering/RenderPipeline.h"
#include "../Core/Camera/CameraController.h"
#include "../Systems/RenderSystem/RenderSystem.h"
#include "Scene/Scene.h"
//#include "../"


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

using namespace Microsoft::WRL;

class EngineCore
{
public:
    
    std::unique_ptr<WinInput> wInput;
    std::unique_ptr<DisplayWin32> window;
    std::unique_ptr<RenderPipeline> renderPipeline;
    std::unique_ptr<CameraController> cameraController;
    std::chrono::time_point<std::chrono::steady_clock> PrevTime;
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    ComPtr <IDXGISwapChain> swapChain;
    ComPtr<ID3D11RenderTargetView> rtv;
    ComPtr < ID3D11Texture2D> backTex;
    ComPtr < ID3D11RasterizerState> rastState;
    ComPtr <ID3D11Texture2D> depthStencilBuffer;
    ComPtr <ID3D11DepthStencilView> depthStencilView;
    ComPtr <ID3D11DepthStencilState> depthStencilState;

    std::unique_ptr<Scene> scene;
    

    //std::map<

    float totalTime = 0;
    unsigned int frameCount = 0;
    float deltaTime = 0;

    static EngineCore* instance()
    {
        static EngineCore inst;
        return &inst;
    }

    EngineCore() = default;
    EngineCore(LPCWSTR appName, HINSTANCE hInstance, const int& width, const int& height);
    ~EngineCore() {};

    HWND GetWindowHWND();
    
    void CreateDeviceAndSwapChain();

    void InitializeDirectX();

    void StartUp();

    void ShutDown();

    void StartUpdateLoop();

    

protected:
    

    void GetInput();
    void Render();
    void Update();

    
};

//TODO: освободить все ресурсы при завершении.