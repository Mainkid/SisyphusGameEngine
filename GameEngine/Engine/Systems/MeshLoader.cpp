#include "MeshLoader.h"
#include "../Core/ServiceLocator.h"
#include "../Systems/HardwareContext.h"
#include <filesystem>

std::shared_ptr<Mesh> MeshLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<DirectX::SimpleMath::Vector4> vertices;
	std::vector<int> indices;

	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		DirectX::XMFLOAT4 vertex;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT4 normals = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1);
		DirectX::XMFLOAT4 tangents = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1);
		DirectX::XMFLOAT4 bitangents= DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1);
		vertex.x = mesh->mVertices[i].x;
		vertex.y = mesh->mVertices[i].y;
		vertex.z = mesh->mVertices[i].z;
		vertex.w = 1.0f;

		if (mesh->mTextureCoords[0])
		{
			color.x = (float)mesh->mTextureCoords[0][i].x;
			color.y = (float)mesh->mTextureCoords[0][i].y;
		}

		if (mesh->mNormals)
		{
			normals.x = mesh->mNormals[i].x;
			normals.y = mesh->mNormals[i].y;
			normals.z = mesh->mNormals[i].z;
		}
		else
		{
			normals.x = 1;
			normals.y = 0;
			normals.z = 0;
		}

		if (mesh->mTangents)
		{
			tangents.x = mesh->mTangents[i].x;
			tangents.y = mesh->mTangents[i].y;
			tangents.z = mesh->mTangents[i].z;
		}

		if (mesh->mBitangents)
		{
			bitangents.x = mesh->mBitangents[i].x;
			bitangents.y = mesh->mBitangents[i].y;
			bitangents.z = mesh->mBitangents[i].z;
		}

		

		vertices.push_back(vertex);
		vertices.push_back(DirectX::XMFLOAT4(color.x, color.y, 1.0f, 1));
		vertices.push_back(normals);
		vertices.push_back(tangents);
		vertices.push_back(bitangents);

	}

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	
	
	auto msh = std::make_shared<Mesh>(vertices, indices);
	
	return  msh;
}

void MeshLoader::ProcessNode(const std::string& modelPath, std::vector<std::shared_ptr<Mesh>>& meshes, aiNode* node, const aiScene* scene)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(MeshLoader::ProcessMesh(mesh, scene));
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(modelPath,meshes, node->mChildren[i], scene);
	}
}

void MeshLoader::LoadTexture(const std::string& texturePath,ID3D11SamplerState** samplerState,
	 ID3D11ShaderResourceView** texture,bool isSRGB)
{
	HardwareContext* hc = ServiceLocator::instance()->Get<HardwareContext>();
	
	if (texturePath == "")
		return;

	if (samplerState != nullptr)
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
		HRESULT hr = hc->device->CreateSamplerState(&sampDesc, samplerState); //Create sampler state
		if (FAILED(hr))
		{
			std::cout << "Texture Loading Failed!" << std::endl;
		}
	}

	std::wstring stemp = std::wstring(texturePath.begin(), texturePath.end());
	LPCWSTR sw = stemp.c_str();
	if (!isSRGB)
		HRESULT hr = DirectX::CreateWICTextureFromFile(hc->device.Get(), sw, nullptr, texture);
	else
		HRESULT hr = DirectX::CreateWICTextureFromFileEx(hc->device.Get(), sw, 20000000, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, DirectX::WIC_LOADER_FORCE_SRGB, nullptr, texture);
	int q = 0;
}

void MeshLoader::LoadModel(const std::string& modelPath, std::vector<std::shared_ptr<Mesh>>& meshes)
{

	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(modelPath, aiProcess_Triangulate | 
		aiProcess_SortByPType | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast);

	if (pScene == nullptr)
		return;

	ProcessNode(modelPath,meshes, pScene->mRootNode, pScene);
}

std::shared_ptr<Mesh> MeshLoader::LoadSimpleMesh(const std::string& modelPath)
{
	std::vector<std::shared_ptr<Mesh>> meshes;
	LoadModel(modelPath, meshes);
	return meshes.at(0);
}
