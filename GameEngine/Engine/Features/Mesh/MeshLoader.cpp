#include "MeshLoader.h"
#include "../../Core/ServiceLocator.h"
#include "../../Contexts/HardwareContext.h"
#include <fstream>
#include <filesystem>
#include "json.hpp"

#include "../../Core/Tools/ErrorLogger.h"
#include "..\Resources\ResourceInfo.h"
#include "../../../vendor/skeletalAnim/AssimpConverter/AssimpConverter.h"
#include "../../../vendor/skeletalAnim/SkeletalAnimation/SkeletalModel.h"



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

SyResult MeshLoader::LoadTexture(const std::string& texturePath,ID3D11SamplerState** samplerState,
	 ID3D11ShaderResourceView** texture,bool isSRGB)
{
	HardwareContext* hc = ServiceLocator::instance()->Get<HardwareContext>();
	
	if (texturePath == "")
		return SyResult();

	std::ifstream in_file;
	std::string textureMetaPath = texturePath + ".meta";
	in_file.open(textureMetaPath);
	nlohmann::json metaData;
	in_file >> metaData;

	

	if (samplerState != nullptr)
	{

		D3D11_SAMPLER_DESC sampDesc;
		D3D11_TEXTURE_ADDRESS_MODE textureMode = D3D11_TEXTURE_ADDRESS_WRAP;
		ZeroMemory(&sampDesc, sizeof(sampDesc));

		if (static_cast<EFilterMode>(metaData["FilterMode"]) == EFilterMode::FILTER_BILINEAR)
			sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		else if (static_cast<EFilterMode>(metaData["FilterMode"]) == EFilterMode::FILTER_POINT)
			sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		else if (static_cast<EFilterMode>(metaData["FilterMode"]) == EFilterMode::FILTER_ANISOTROPIC)
			sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;

		if (static_cast<EWrapMode>(metaData["WrapMode"]) == EWrapMode::WRAP_CLAMP)
			textureMode = D3D11_TEXTURE_ADDRESS_CLAMP;
		else if (static_cast<EWrapMode>(metaData["WrapMode"]) == EWrapMode::WRAP_MIRROR)
			textureMode = D3D11_TEXTURE_ADDRESS_MIRROR;
		else
			textureMode = D3D11_TEXTURE_ADDRESS_WRAP;

		sampDesc.AddressU = textureMode;
		sampDesc.AddressV = textureMode;
		sampDesc.AddressW = textureMode;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		HRESULT hr = hc->device->CreateSamplerState(&sampDesc, samplerState); //Create sampler state
		if (FAILED(hr))
		{
			return SyResult(SY_RESCODE_ERROR, "Texture Loading Failed!");

		}
	}

	std::wstring stemp = std::wstring(texturePath.begin(), texturePath.end());
	LPCWSTR sw = stemp.c_str();
	if (!metaData["sRGB"])
		HRESULT hr = DirectX::CreateWICTextureFromFile(hc->device.Get(), sw, nullptr, texture);
	else
		HRESULT hr = DirectX::CreateWICTextureFromFileEx(hc->device.Get(), sw, 20000000, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, DirectX::WIC_LOADER_FORCE_SRGB, nullptr, texture);
	int q = 0;

	return SyResult();
}

void MeshLoader::LoadModel(const std::string& modelPath, std::vector<std::shared_ptr<Mesh>>& meshes, std::shared_ptr<SA::SkeletalModel> skeleton)
{

	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(modelPath, aiProcess_Triangulate | 
		aiProcess_SortByPType | aiProcess_LimitBoneWeights |
		aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast);

	
	if (skeleton != nullptr)
		AssimpConverter::Convert(pScene, *skeleton.get());

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
