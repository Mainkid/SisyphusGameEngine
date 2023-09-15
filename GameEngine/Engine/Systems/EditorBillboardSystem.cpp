#include "EditorBillboardSystem.h"
#include "../Core/ServiceLocator.h"
#include "../Components/EditorBillboardComponent.h"
#include "MeshLoader.h"
#include "EngineContext.h"

void EditorBillboardSystem::Init()
{
	ec = ServiceLocator::instance()->Get<EngineContext>();
}

void EditorBillboardSystem::Run()
{
	auto view = ec->scene->registry.view<EditorBillboardComponent>();
	for (auto& entity : view)
	{
		EditorBillboardComponent& ebc = view.get<EditorBillboardComponent>(entity);
		uint32_t hsh = hasher(ebc.texturePath);
		if (ebc.hash != hsh)
		{
			ebc.hash = hasher(ebc.texturePath);
			MeshLoader::LoadTexture(ebc.texturePath, ebc.samplerState.GetAddressOf(), ebc.texture.GetAddressOf());
		}
	}
}

void EditorBillboardSystem::Destroy()
{
}
