#include "MeshSystem.h"
#include "../Components/MeshComponent.h"
#include "../Core/ServiceLocator.h"
#include "../Systems/HardwareContext.h"
#include "../Systems/EngineContext.h"
#include "../Systems/ResourceService.h"
#include "MeshLoader.h"

void MeshSystem::Init()
{
	hc = ServiceLocator::instance()->Get<HardwareContext>();
	ec = ServiceLocator::instance()->Get<EngineContext>();
	rs = ServiceLocator::instance()->Get<ResourceService>();
}

void MeshSystem::Run()
{
	auto view = ec->scene->registry.view<MeshComponent>();
	for (auto& entity : view)
	{
		MeshComponent& mesh =view.get<MeshComponent>(entity);
		uint32_t hsh = hasher(mesh.modelUUID);
		if (mesh.hash != hsh)
		{
			mesh.hash = hasher(mesh.modelUUID);
			mesh.model=static_cast<Model*>( rs->LoadResource(mesh.modelUUID));
			mesh.materials.resize(mesh.model->meshes.size());

			for (int i = 0; i < mesh.materials.size(); i++)
			{
				if (mesh.materials[i] == nullptr)
				{
					mesh.materials[i] = static_cast<Material*>(rs->baseResourceDB[EAssetType::ASSET_MATERIAL]);
				}
			}

		}
	}
}

void MeshSystem::Destroy()
{
}

