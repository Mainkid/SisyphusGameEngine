#include "MeshSystem.h"
#include "../Components/MeshComponent.h"
#include "../Core/ServiceLocator.h"
#include "../Systems/HardwareContext.h"
#include "../Systems/EngineContext.h"
#include "MeshLoader.h"

SyResult MeshSystem::Init()
{
	hc = ServiceLocator::instance()->Get<HardwareContext>();
	ec = ServiceLocator::instance()->Get<EngineContext>();
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "Mesh system initialization successful.");
	return SyResult();
}

SyResult MeshSystem::Run()
{
	auto view = ec->scene->registry.view<MeshComponent>();
	for (auto& entity : view)
	{
		MeshComponent& mesh =view.get<MeshComponent>(entity);
		uint32_t hsh = hasher(mesh.modelPath + mesh.texturePath);
		if (mesh.hash != hsh)
		{
			mesh.hash = hasher(mesh.modelPath + mesh.texturePath);
			mesh.meshes.clear();
			//LoadModel(mesh);
			//LoadTexture(mesh);
			MeshLoader::LoadModel(mesh.modelPath, mesh.meshes);
			MeshLoader::LoadTexture(mesh.texturePath, mesh.samplerState.GetAddressOf(), mesh.texture.GetAddressOf());
		}
	}
	return SyResult();
}

SyResult MeshSystem::Destroy()
{
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "Mesh system destruction successful.");
	return SyResult();
}

bool MeshSystem::LoadModel(MeshComponent& mesh)
{
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(mesh.modelPath, aiProcess_Triangulate |
		aiProcess_SortByPType | aiProcess_ConvertToLeftHanded);

	if (pScene == nullptr)
		return false;

	this->ProcessNode(mesh,pScene->mRootNode, pScene);

	return true;
}

void MeshSystem::LoadTexture(MeshComponent& mesh)
{
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT hr =hc->device->CreateSamplerState(&sampDesc,mesh.samplerState.GetAddressOf()); //Create sampler state
	if (FAILED(hr))
	{
		std::cout << "Texture Loading Failed!" << std::endl;
	}

	std::wstring stemp = std::wstring(mesh.texturePath.begin(), mesh.texturePath.end());
	LPCWSTR sw = stemp.c_str();
	hr = DirectX::CreateWICTextureFromFile(hc->device.Get(),sw, nullptr, mesh.texture.GetAddressOf());
}

void MeshSystem::ProcessNode(MeshComponent& meshComp,aiNode* node, const aiScene* scene)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshComp.meshes.push_back(MeshLoader::ProcessMesh(mesh, scene));
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessNode(meshComp,node->mChildren[i], scene);
	}
}


