#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <WinUser.h>
#include <wrl.h>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include "../Core/IService.h"
#include "../Core/DisplayWin32.h"
#include "../Core/Rendering/RenderTarget.h"

using namespace Microsoft::WRL;
struct HardwareContext : public IService
{
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    ComPtr <IDXGISwapChain> swapChain;
    ComPtr<ID3D11RenderTargetView> rtv;
    ComPtr <ID3D11Texture2D> backTex;
    ComPtr <ID3D11RasterizerState> rastState;
    ComPtr <ID3D11Texture2D> depthStencilBuffer;
    ComPtr <ID3D11DepthStencilView> depthStencilView;
    ComPtr <ID3D11DepthStencilState> depthStencilState;
    std::unique_ptr<RenderTarget> renderTarget;
    std::unique_ptr<DisplayWin32> window;
};

