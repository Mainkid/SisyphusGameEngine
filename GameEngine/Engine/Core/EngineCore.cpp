#include "EngineCore.h"
#include "../Components/EditorBillboardComponent.h"
#include "../Components/GameObjectComp.h"
#include "../Components/MeshComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/ImageBasedLightingComponent.h"


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
	ServiceLocator::instance()->Register<RenderContext>();
	ServiceLocator::instance()->Register<HardwareContext>();
	ServiceLocator::instance()->Register<SyErrorLogger>();
	ServiceLocator::instance()->Register<EngineContext>();
	ServiceLocator::instance()->Register<ResourceService>();
	_context = ServiceLocator::instance()->Get<EngineContext>();


	ser::Serializer& ser = ServiceLocator::instance()->Get<EngineContext>()->serializer;
	ser.AddEcsCompMeta<GameObjectComp>();
	ser.AddEcsCompMeta<TransformComponent>();
	ser.AddEcsCompMeta<MeshComponent>();
	ser.AddEcsCompMeta<LightComponent>();
	ser.AddEcsCompMeta<EditorBillboardComponent>();
	ser.AddEcsCompMeta<SkyboxComponent>();
	ser.AddEcsCompMeta<ImageBasedLightingComponent>();
	ser.AddEcsCompMeta<ParticleComponent>();
	//ser.AddEcsCompMeta<SharedParticlesData>();


	
	ServiceLocator::instance()->Register<mono::SyMono>();
	mono::SyMono* mono = ServiceLocator::instance()->Get<mono::SyMono>();
	mono->Init();
	mono->HotReload();

	StartUpSystems();
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "All systems initialization complete!");
}

void EngineCore::StartUpSystems()
{
	_systems.AttachEcs(&_context->ecs);

	_systems.Add<HardwareInitSystem>();

	_systems.Add<RenderInitSystem>();

	
	_systems.Add<InputSystem>();
	_systems.Add<ResourceSystem>();
	_systems.Add<SyRBodySystem>();
	_systems.Add<SyCollisionSystem>();


	_systems.Add<TransformSystem>();

	_systems.Add<EditorCameraSystem>();

	_systems.Add<MeshSystem>();

	_systems.Add<LightSystem>();
	_systems.Add<MeshSystem>();
	_systems.Add<SkyboxSystem>();
	_systems.Add<LightSystem>();
	_systems.Add<ParticlesSystem>();
	_systems.Add<EditorBillboardSystem>();

	_systems.Add<MonoSyncEgSystem>();

	_systems.Add<PreRenderSystem>();
	_systems.Add<ShadowRenderSystem>();
	_systems.Add<SkyboxRenderSystem>();
	_systems.Add<ImageBasedLightingSystem>();
	_systems.Add<OpaqueRenderSystem>();
	_systems.Add<HbaoRenderSystem>();
	_systems.Add<ShadowMapGenerationSystem>();
	_systems.Add<LightRenderSystem>();
	_systems.Add<ParticleRenderSystem>();
	_systems.Add<EditorBillboardRenderSystem>();
	_systems.Add<ToneMappingRenderSystem>();
	_systems.Add<EditorGridRenderSystem>();
	_systems.Add<PostViewportRenderSystem>();

	_systems.Add<HudPreRenderSystem>();
	_systems.Add<HudContentBrowserSystem>();
	_systems.Add<HudHierarchySystem>();
	_systems.Add<HudPropertiesSystem>();
	_systems.Add<HudViewportSystem>();
	_systems.Add<SyHudConsoleSystem>();
	_systems.Add<SyErrorLoggingSystem>();
	_systems.Add<HudPostRenderSystem>();
	_systems.Add<SyPrepareEventsSystem>();

	_systems.Add<MonoSyncGeSystem>();
	
	_systems.Init();
}

void EngineCore::ShutDown()
{
	_systems.Destroy();

	ServiceLocator::instance()->Unregister<EngineCore>();
	ServiceLocator::instance()->Unregister<RenderContext>();
	ServiceLocator::instance()->Unregister<HardwareContext>();
	ServiceLocator::instance()->Unregister<mono::SyMono>();
	ServiceLocator::instance()->Unregister<SyErrorLogger>();
	ServiceLocator::instance()->Unregister<ResourceService>();
}

