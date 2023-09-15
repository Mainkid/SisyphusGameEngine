#include "EditorBillboardSystem.h"
#include "../Core/EngineCore.h"

void EditorBillboardSystem::Init()
{
}

void EditorBillboardSystem::Run()
{
	auto view = EngineCore::instance()->scene->registry.view<EditorBillboardComponent>();
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
