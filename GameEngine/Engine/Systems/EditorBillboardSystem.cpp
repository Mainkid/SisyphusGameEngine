#include "EditorBillboardSystem.h"
#include "../Core/ServiceLocator.h"
#include "../Components/EditorBillboardComponent.h"
#include "MeshLoader.h"
#include "EngineContext.h"

SyResult EditorBillboardSystem::Init()
{
	_ec = ServiceLocator::instance()->Get<EngineContext>();
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
			SyResult hr = MeshLoader::LoadTexture(ebc.texturePath, ebc.samplerState.GetAddressOf(), ebc.texture.GetAddressOf(), true);
			if (hr.code != SY_RESCODE_OK)
			{
				SY_LOG_REND(SY_LOGLEVEL_ERROR, hr.message.ToString());
			}
		}
	}
	return SyResult();
}

SyResult EditorBillboardSystem::Destroy()
{
	return SyResult();
}
