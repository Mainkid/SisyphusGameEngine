#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <WinUser.h>
#include <wrl.h>
#include <iostream>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>



struct HardwareContext;

using namespace Microsoft::WRL;
class RenderTarget
{
public:
	RenderTarget();
	bool Initialize(int width = 1280, int height = 720);
	void Close();
	void SetRenderTarget(ID3D11DepthStencilView* depthStencilView);
	void ClearRenderTarget(ID3D11DepthStencilView* depthStencilView, D3D11_CLEAR_FLAG flags);
	~RenderTarget()
	{

	};
	ID3D11ShaderResourceView* GetSRV();
	ID3D11ShaderResourceView* GetImageSRV();
	ComPtr<ID3D11RenderTargetView> renderTargetView;
private:
	ComPtr<ID3D11Texture2D> renderTargetTexture;
	ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	ComPtr<ID3D11ShaderResourceView> ImageSRV;

	HardwareContext* hc;

};

