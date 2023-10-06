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


void EngineCore::StartUpdateLoop()
{
	PrevTime = std::chrono::steady_clock::now();
	while (true) {
		Update();
	}
}




void EngineCore::Render()
{
	
}

void EngineCore::Update()
{
	auto	curTime = std::chrono::steady_clock::now();
	ec->deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - PrevTime).count() / 1000000.0f;
	PrevTime = curTime;

	ec->totalTime += ec->deltaTime;
	ec->frameCount++;

	if (ec->totalTime > 1.0f) {
		float fps = ec->frameCount / ec->totalTime;

		ec->totalTime -= 1.0f;
		ec->frameCount = 0;
	}

	for (auto& system : systems)
	{
		system->Run();
	}
	HardwareContext* hc=ServiceLocator::instance()->Get<HardwareContext>();
	hc->swapChain->Present(1, 0);
	ec->scene->Update(ec->deltaTime);
}

void EngineCore::StartUpSystems()
{
	ServiceLocator::instance()->Register<EngineContext>();
	ServiceLocator::instance()->Register<ResourceService>();
	ec = ServiceLocator::instance()->Get<EngineContext>();

	std::unique_ptr<HardwareInitSystem> his = std::make_unique < HardwareInitSystem>();
	systems.push_back(std::move(his));

	std::unique_ptr<RenderInitSystem> ris = std::make_unique<RenderInitSystem>();
	systems.push_back(std::move(ris));

	std::unique_ptr<InputSystem> is = std::make_unique < InputSystem>();
	systems.push_back(std::move(is));

	std::unique_ptr<ResourceSystem> ress= std::make_unique < ResourceSystem>();
	systems.push_back(std::move(ress));

	std::unique_ptr<SyPhysicsSystem> ps = std::make_unique<SyPhysicsSystem>();
	systems.push_back(std::move(ps));

	std::unique_ptr<TransformSystem> ts= std::make_unique<TransformSystem>();
	systems.push_back(std::move(ts));

	

	std::unique_ptr<EditorCameraSystem> ecs = std::make_unique<EditorCameraSystem>();
	systems.push_back(std::move(ecs));

	std::unique_ptr<MeshSystem> ms = std::make_unique<MeshSystem>();
	systems.push_back(std::move(ms));

	std::unique_ptr<MaterialUpdateSystem> mus = std::make_unique<MaterialUpdateSystem>();
	systems.push_back(std::move(mus));

	std::unique_ptr<LightSystem> ls = std::make_unique<LightSystem>();
	systems.push_back(std::move(ls));

	std::unique_ptr<EditorBillboardSystem> ebs = std::make_unique<EditorBillboardSystem>();
	systems.push_back(std::move(ebs));

	std::unique_ptr<PreRenderSystem> prs = std::make_unique<PreRenderSystem>();
	systems.push_back(std::move(prs));

	

	std::unique_ptr<ShadowRenderSystem> srs = std::make_unique<ShadowRenderSystem>();
	systems.push_back(std::move(srs));

	std::unique_ptr<SkyboxRenderSystem> srs2 = std::make_unique<SkyboxRenderSystem>();
	systems.push_back(std::move(srs2));

	std::unique_ptr<OpaqueRenderSystem> ors = std::make_unique<OpaqueRenderSystem>();
	systems.push_back(std::move(ors));

	std::unique_ptr<ShadowMapGenerationSystem> sgs = std::make_unique<ShadowMapGenerationSystem>();
	systems.push_back(std::move(sgs));

	std::unique_ptr<LightRenderSystem> lrs = std::make_unique<LightRenderSystem>();
	systems.push_back(std::move(lrs));

	std::unique_ptr<EditorBillboardRenderSystem> brs = std::make_unique<EditorBillboardRenderSystem>();
	systems.push_back(std::move(brs));

	std::unique_ptr<ToneMappingRenderSystem> tmrs = std::make_unique<ToneMappingRenderSystem>();
	systems.push_back(std::move(tmrs));

	std::unique_ptr<PostViewportRenderSystem> postrs = std::make_unique<PostViewportRenderSystem>();
	systems.push_back(std::move(postrs));

	//HUD
	std::unique_ptr<HudPreRenderSystem> hprs = std::make_unique<HudPreRenderSystem>();
	systems.push_back(std::move(hprs));

	std::unique_ptr<HudContentBrowserSystem> hcbs = std::make_unique<HudContentBrowserSystem>();
	systems.push_back(std::move(hcbs));

	std::unique_ptr<HudHierarchySystem> hhs= std::make_unique<HudHierarchySystem>();
	systems.push_back(std::move(hhs));

	std::unique_ptr<HudPropertiesSystem> hps= std::make_unique<HudPropertiesSystem>();
	systems.push_back(std::move(hps));

	std::unique_ptr<HudViewportSystem> hvs= std::make_unique<HudViewportSystem>();
	systems.push_back(std::move(hvs));
	
	std::unique_ptr<HudPostRenderSystem> hpostrs= std::make_unique<HudPostRenderSystem>();
	systems.push_back(std::move(hpostrs));
	
	for (const auto& system : systems)
	{
		system->Init();
	}
}

void EngineCore::StartUp()
{
	StartUpSystems();
}

void EngineCore::ShutDown()
{
	for (auto& system : systems)
	{
		system->Destroy();
	}
}
