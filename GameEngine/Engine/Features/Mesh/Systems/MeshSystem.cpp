#include "MeshSystem.h"
#include "../Components/MeshComponent.h"
#include "../../../Core/ServiceLocator.h"
#include "../../../Contexts/EngineContext.h"
#include "../../../Contexts/HardwareContext.h"
#include "../../Resources/ResourceService.h"
#include "../MeshLoader.h"

SyResult MeshSystem::Init()
{
	hc = ServiceLocator::instance()->Get<HardwareContext>();
	ec = ServiceLocator::instance()->Get<EngineContext>();
	rs = ServiceLocator::instance()->Get<ResourceService>();
	return SyResult();
}

SyResult MeshSystem::Run()
{
	auto view = _ecs->view<MeshComponent>();
	for (auto& entity : view)
	{
		MeshComponent& mesh =view.get<MeshComponent>(entity);

		uint32_t hash = hasherModelUuid(mesh.modelUUID);
		if (mesh.hashModelUuid != hash)
		{
			mesh.hashModelUuid = hash;
			mesh.model=std::static_pointer_cast<Model>( rs->LoadResource(mesh.modelUUID));
			mesh.materials.resize(mesh.model->meshes.size());
			mesh.materialUUIDs.resize(mesh.model->meshes.size());
			
			for (int i = 0; i < mesh.materials.size(); i++)
			{
				if (mesh.materialUUIDs[i]==boost::uuids::nil_uuid())
					mesh.materialUUIDs[i] = (rs->baseResourceDB[EAssetType::ASSET_MATERIAL].uuid);
			}
		}

		hash = hasherMaterialsUUids(mesh.materialUUIDs);
		if (mesh.hashMaterialUuid != hash)
		{
			if (mesh.materialUUIDs.size() != mesh.materials.size())
			{
				mesh.materialUUIDs.resize(mesh.materials.size());
				hash = hasherMaterialsUUids(mesh.materialUUIDs);
			}
			mesh.hashMaterialUuid = hash;

			for (int i = 0; i < mesh.materialUUIDs.size(); i++)
			{
				//rs->UnloadResource(mesh.materials[i]);
				mesh.materials[i] = std::static_pointer_cast<Material>(rs->LoadResource(mesh.materialUUIDs[i]));
			}
		}

	}

	return SyResult();
}

SyResult MeshSystem::Destroy()
{
	return SyResult();
}

