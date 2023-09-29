#include "HardwareInitSystem.h"
#include "../Core/DisplayWin32.h"
#include "../Core/Rendering/RenderTarget.h"

void HardwareInitSystem::Init()
{
	ServiceLocator::instance()->Register<HardwareContext>();
	hc = ServiceLocator::instance()->Get<HardwareContext>();
	CreateOSWindow(L"Sisyphus", GetModuleHandle(nullptr), 1280, 720);
	CreateDeviceAndSwapChain();
	InitializeDirectX();
	hc->renderTarget = std::make_unique<RenderTarget>();
	hc->renderTarget->Initialize();
}

void HardwareInitSystem::Run()
{

}

void HardwareInitSystem::Destroy()
{

}


void HardwareInitSystem::CreateDeviceAndSwapChain()
{
	DXGI_SWAP_CHAIN_DESC swapDesc;
	swapDesc = {};
	swapDesc.BufferCount = 2; 
	swapDesc.BufferDesc.Width = hc->window->GetWidth();
	swapDesc.BufferDesc.Height = hc->window->GetHeight();
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = hc->window->GetHWND();
	swapDesc.Windowed = true;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;


	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));


	D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 };

	HRESULT res = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		featureLevel,
		1,
		D3D11_SDK_VERSION,
		&swapDesc,
		&hc->swapChain,
		&hc->device,
		nullptr,
		&hc->context);

	D3D11_RENDER_TARGET_VIEW_DESC desc;

	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice = 0;

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = hc->window->GetWidth();
	depthStencilDesc.Height = hc->window->GetHeight();
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	HRESULT hr = hc->device->CreateTexture2D(&depthStencilDesc, NULL, hc->depthStencilBuffer.GetAddressOf());


	hr = hc->device->CreateDepthStencilView(hc->depthStencilBuffer.Get(), NULL, hc->depthStencilView.GetAddressOf());

	

	res = hc->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)hc->backTex.GetAddressOf());	// __uuidof(ID3D11Texture2D)
	res = hc->device->CreateRenderTargetView(hc->backTex.Get(), nullptr, hc->rtv.GetAddressOf());

}

void HardwareInitSystem::InitializeDirectX()
{

	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(1280);
	viewport.Height = static_cast<float>(720);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	hc->context->RSSetViewports(1, &viewport);
	hc->renderTarget = std::make_unique<RenderTarget>();
}

void HardwareInitSystem::CreateOSWindow(LPCWSTR appName, HINSTANCE hInstance, const int& width, const int& height)
{
	hc->window = std::make_unique<DisplayWin32>(appName, hInstance, width, height);
}
