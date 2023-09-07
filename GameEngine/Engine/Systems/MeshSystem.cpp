#include "MeshSystem.h"
#include "../Components/MeshComponent.h"
#include "../Core/EngineCore.h"

void MeshSystem::Init()
{
}

void MeshSystem::Run()
{
	for (auto& entity : EngineCore::instance()->scene->registry.view<MeshComponent>())
	{
		MeshComponent& mesh = EngineCore::instance()->scene->registry.get<MeshComponent>(entity);
		uint32_t hsh = hasher(mesh.modelPath + mesh.texturePath);
		if (mesh.hash != hsh)
		{
			mesh.hash = hasher(mesh.modelPath + mesh.texturePath);
			mesh.meshes.clear();
			LoadModel(mesh);
			LoadTexture(mesh);
		}
	}
}

void MeshSystem::Destroy()
{
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
	HRESULT hr = EngineCore::instance()->device->CreateSamplerState(&sampDesc,mesh.samplerState.GetAddressOf()); //Create sampler state
	if (FAILED(hr))
	{
		std::cout << "Texture Loading Failed!" << std::endl;
	}

	//D3D11_TEXTURE2D
	//hr=DirectX::CreateWICTextureFromFileEx(game->device.Get(),texturePath,2000000,D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE,0,0,DirectX::WIC_LOADER_FORCE_SRGB,nullptr,texture.GetAddressOf());
	
	std::wstring stemp = std::wstring(mesh.texturePath.begin(), mesh.texturePath.end());
	LPCWSTR sw = stemp.c_str();
	hr = DirectX::CreateWICTextureFromFile(EngineCore::instance()->device.Get(),sw, nullptr, mesh.texture.GetAddressOf());
}

void MeshSystem::ProcessNode(MeshComponent& meshComp,aiNode* node, const aiScene* scene)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshComp.meshes.push_back(this->ProcessMesh(mesh, scene));
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessNode(meshComp,node->mChildren[i], scene);
	}
}

std::shared_ptr<Mesh> MeshSystem::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<DirectX::SimpleMath::Vector4> vertices;
	std::vector<int> indices;

	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		DirectX::XMFLOAT4 vertex;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT4 normals = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex.x = mesh->mVertices[i].x;
		vertex.y = mesh->mVertices[i].y;
		vertex.z = mesh->mVertices[i].z;
		vertex.w = 1.0f;

		if (mesh->mTextureCoords[0])
		{
			color.x = (float)mesh->mTextureCoords[0][i].x;
			color.y = (float)mesh->mTextureCoords[0][i].y;
		}

		normals.x = mesh->mNormals[i].x;
		normals.y = mesh->mNormals[i].y;
		normals.z = mesh->mNormals[i].z;


		vertices.push_back(vertex); 
		vertices.push_back(DirectX::XMFLOAT4(color.x, color.y, 1.0f, 1));
		vertices.push_back(normals);

	}

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	return  std::make_shared<Mesh>(vertices, indices);
}
