#include "EditorBillboardSystem.h"
#include "../Core/ServiceLocator.h"
#include "../Components/EditorBillboardComponent.h"
#include "MeshLoader.h"
#include "EngineContext.h"

SyResult EditorBillboardSystem::Init()
{
	_ec = ServiceLocator::instance()->Get<EngineContext>();
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "EditorBillboard system initialization successful. ");
	return SyResult();
}

SyResult EditorBillboardSystem::Run()
{
	auto view = _ec->ecs.view<EditorBillboardComponent>();
	for (auto& entity : view)
	{
		EditorBillboardComponent& ebc = view.get<EditorBillboardComponent>(entity);
		uint32_t hsh = _hasher(ebc.texturePath);
		if (ebc.hash != hsh)
		{
			ebc.hash = _hasher(ebc.texturePath);
			MeshLoader::LoadTexture(ebc.texturePath, ebc.samplerState.GetAddressOf(), ebc.texture.GetAddressOf(),true);
		}
	}
	return SyResult();
}

SyResult EditorBillboardSystem::Destroy()
{
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "EditorBillboard system destruction successful. ");
	return SyResult();
}
