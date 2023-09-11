#include "EngineCore.h"

EngineCore::EngineCore(LPCWSTR appName, HINSTANCE hInstance, const int& width, const int& height)
{
	window = std::make_unique<DisplayWin32>(appName, hInstance, width, height);
	wInput = std::make_unique<WinInput>();
}

HWND EngineCore::GetWindowHWND()
{
	return HWND();
}

void EngineCore::CreateDeviceAndSwapChain()
{
	DXGI_SWAP_CHAIN_DESC swapDesc;
	swapDesc = {};
	swapDesc.BufferCount = 2;
	swapDesc.BufferDesc.Width = window->GetWidth();
	swapDesc.BufferDesc.Height = window->GetHeight();
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = window->GetHWND();
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
		&swapChain,
		&device,
		nullptr,
		&context);

	D3D11_RENDER_TARGET_VIEW_DESC desc;

	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice = 0;

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = window->GetWidth();
	depthStencilDesc.Height = window->GetHeight();
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	HRESULT hr = this->device->CreateTexture2D(&depthStencilDesc, NULL, this->depthStencilBuffer.GetAddressOf());


	hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());



	res = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backTex.GetAddressOf());	// __uuidof(ID3D11Texture2D)
	res = device->CreateRenderTargetView(backTex.Get(), nullptr, rtv.GetAddressOf());

}

void EngineCore::InitializeDirectX()
{
	window = std::make_unique<DisplayWin32>( L"Untitled", GetModuleHandle(nullptr), 1280, 720);
	wInput = std::make_unique<WinInput>();
	cameraController = std::make_unique<CameraController>();
	CreateDeviceAndSwapChain();

	scene = std::make_unique<Scene>();

	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(1280);
	viewport.Height = static_cast<float>(720);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	context->RSSetViewports(1, &viewport);
	renderTarget = std::make_unique<RenderTarget>();
	renderPipeline = std::make_unique<RenderPipeline>(this);
	hud = std::make_unique<Hud>();
	

}

void EngineCore::StartUpdateLoop()
{
	PrevTime = std::chrono::steady_clock::now();

	while (wInput->ProcessMessages()) {

		Update();
		GetInput();
		Render();

	}
}

void EngineCore::GetInput()
{
	wInput.get()->GetInput();
}

void EngineCore::Render()
{

	renderPipeline->Render();
	hud->Render();
	swapChain->Present(1, 0);
}

void EngineCore::Update()
{
	auto	curTime = std::chrono::steady_clock::now();
	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - PrevTime).count() / 1000000.0f;
	PrevTime = curTime;

	totalTime += deltaTime;
	frameCount++;

	if (totalTime > 1.0f) {
		float fps = frameCount / totalTime;

		totalTime -= 1.0f;
		frameCount = 0;
	}

	for (auto& system : systems)
	{
		system->Run();
	}
	scene->Update(deltaTime);
	cameraController->CameraMovement(deltaTime);
}

void EngineCore::StartUpSystems()
{
	std::unique_ptr<TransformSystem> ts= std::make_unique<TransformSystem>();
	systems.push_back(std::move(ts));

	std::unique_ptr<EditorCameraSystem> ecs = std::make_unique<EditorCameraSystem>();
	systems.push_back(std::move(ecs));

	std::unique_ptr<MeshSystem> ms = std::make_unique<MeshSystem>();
	systems.push_back(std::move(ms));

	std::unique_ptr<LightSystem> ls = std::make_unique<LightSystem>();
	systems.push_back(std::move(ls));


	for (const auto& system : systems)
	{
		system->Init();
	}
}

void EngineCore::StartUp()
{
	InitializeDirectX();
	RenderSystem::instance()->StartUp();
	renderTarget->Initialize(window->GetWidth(),window->GetHeight());
	renderPipeline->Initialize();
	hud->Initialize();
	cameraController->Initialize();

	StartUpSystems();

	for (auto& system : systems)
	{
		system->Init();
	}
}

void EngineCore::ShutDown()
{
	RenderSystem::instance()->ShutDown();
	for (auto& system : systems)
	{
		system->Destroy();
	}
}
