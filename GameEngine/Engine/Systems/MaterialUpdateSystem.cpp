#include "MaterialUpdateSystem.h"
#include "EngineContext.h"
#include "HardwareContext.h"
#include "Rendering/RenderContext.h"
#include "../Core/ServiceLocator.h"
#include "MeshLoader.h"


void MaterialUpdateSystem::Init()
{
	ec = ServiceLocator::instance()->Get<EngineContext>();
	rc = ServiceLocator::instance()->Get<RenderContext>();
	hc = ServiceLocator::instance()->Get<HardwareContext>();

	//rc->materialSet.insert(std::make_unique<Material>());
}

void MaterialUpdateSystem::Run()
{
	const std::string basePath = "";
	auto view = ec->scene->registry.view< MeshComponent>();
	for (auto& entity : view)
	{
		MeshComponent& mesh = view.get<MeshComponent>(entity);
		for (auto& msh : mesh.meshes)
		{
			if (msh->material == nullptr)
			{
				auto it = rc->materialSet.begin();
				msh->material = (*it).get();
			}

			uint32_t hsh = hasher(*msh->material);
			if (hsh != msh->material->hash)
			{
				msh->material->hash = hsh;
				MeshLoader::LoadTexture(basePath+msh->material->albedoTexturePath, nullptr, msh->material->albedoSRV.GetAddressOf(), true);
				MeshLoader::LoadTexture(basePath + msh->material->metallicTexturePath, nullptr, msh->material->metallicSRV.GetAddressOf());
				MeshLoader::LoadTexture(basePath + msh->material->roughnessTexturePath, nullptr, msh->material->roughnessSRV.GetAddressOf());
				MeshLoader::LoadTexture(basePath + msh->material->normalmapTexturePath, nullptr, msh->material->normalmapSRV.GetAddressOf());
				MeshLoader::LoadTexture(basePath + msh->material->emissiveTexturePath, nullptr, msh->material->emissiveSRV.GetAddressOf());
				msh->material->resources[0] = msh->material->albedoSRV.Get();
				msh->material->resources[1] = msh->material->specularSRV.Get();
				msh->material->resources[2] = msh->material->roughnessSRV.Get();
				msh->material->resources[3] = msh->material->metallicSRV.Get();
				msh->material->resources[4] = msh->material->emissiveSRV.Get();
				msh->material->resources[5] = msh->material->normalmapSRV.Get();
				msh->material->resources[6] = msh->material->opacitySRV.Get();
			}
		}
	}
}

void MaterialUpdateSystem::Destroy()
{
}
