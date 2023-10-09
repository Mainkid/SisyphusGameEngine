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
		uint32_t hsh = hasherModel(mesh.modelUUID);
		if (mesh.hashModel != hsh)
		{
			mesh.hashModel = hasherModel(mesh.modelUUID);
			mesh.model=std::static_pointer_cast<Model>( rs->LoadResource(mesh.modelUUID));
			mesh.materials.resize(mesh.model->meshes.size());
			mesh.materialUUIDs.resize(mesh.model->meshes.size());
			
			for (int i = 0; i < mesh.materials.size(); i++)
			{
				mesh.materialUUIDs[i] = (rs->baseResourceDB[EAssetType::ASSET_MATERIAL]);
			}
		}

		hsh = hasherMaterial(mesh.materialUUIDs);
		if (mesh.hashMaterial != hsh)
		{
			mesh.hashMaterial = hasherMaterial(mesh.materialUUIDs);
			for (int i = 0; i < mesh.materialUUIDs.size(); i++)
			{
				//rs->UnloadResource(mesh.materials[i]);
				mesh.materials[i] = std::static_pointer_cast<Material>(rs->LoadResource(mesh.materialUUIDs[i]));
			}
		}

	}
}

void MeshSystem::Destroy()
{
}

