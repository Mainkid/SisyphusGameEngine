#include "EngineCore.h"

//EngineCore::EngineCore(LPCWSTR appName, HINSTANCE hInstance, const int& width, const int& height)
//{
//	window = std::make_unique<DisplayWin32>(appName, hInstance, width, height);
//}

EngineCore::EngineCore()
{
	
}

HWND EngineCore::GetWindowHWND()
{
	return HWND();
}


void EngineCore::Update()
{
	PrevTime = std::chrono::steady_clock::now();
	while (!_context->isClosed) 
	{
		UpdateImpl();
	}
}

void EngineCore::UpdateImpl()
{
	auto	curTime = std::chrono::steady_clock::now();
	_context->deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - PrevTime).count() / 1000000.0f;
	PrevTime = curTime;

	_context->totalTime += _context->deltaTime;
	_context->frameCount++;

	if (_context->totalTime > 1.0f) 
	{
		float fps = _context->frameCount / _context->totalTime;

		_context->totalTime -= 1.0f;
		_context->frameCount = 0;
	}

	_systems.Run();
	
	HardwareContext* hc=ServiceLocator::instance()->Get<HardwareContext>();
	hc->swapChain->Present(1, 0);
}


void EngineCore::StartUp()
{
	ServiceLocator::instance()->Register<EngineContext>();
	_context = ServiceLocator::instance()->Get<EngineContext>();

	StartUpSystems();
}

void EngineCore::StartUpSystems()
{
	_systems.AttachEcs(&_context->ecs);

	_systems.Add<HardwareInitSystem>();

	_systems.Add<RenderInitSystem>();

	_systems.Add<InputSystem>();

	_systems.Add<SyPhysicsSystem>();

	_systems.Add<TransformSystem>();

	_systems.Add<EditorCameraSystem>();

	_systems.Add<MeshSystem>();

	_systems.Add<LightSystem>();

	_systems.Add<EditorBillboardSystem>();

	_systems.Add<PreRenderSystem>();
	_systems.Add<ShadowRenderSystem>();
	_systems.Add<SkyboxRenderSystem>();
	_systems.Add<OpaqueRenderSystem>();
	_systems.Add<ShadowMapGenerationSystem>();
	_systems.Add<LightRenderSystem>();
	_systems.Add<PostViewportRenderSystem>();

	_systems.Add<HudPreRenderSystem>();
	_systems.Add<HudContentBrowserSystem>();
	_systems.Add<HudHierarchySystem>();
	_systems.Add<HudPropertiesSystem>();
	_systems.Add<HudViewportSystem>();
	_systems.Add<HudPostRenderSystem>();

	_systems.Init();
}

void EngineCore::ShutDown()
{
	_systems.Destroy();

	ServiceLocator::instance()->Unregister<EngineCore>();
	ServiceLocator::instance()->Unregister<RenderContext>();
	auto hc = ServiceLocator::instance()->Get<HardwareContext>();
	ServiceLocator::instance()->Unregister<HardwareContext>();
}

