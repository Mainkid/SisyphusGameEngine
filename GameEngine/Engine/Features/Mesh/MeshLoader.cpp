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
#include "../Animations/SkeletalAnimation.h"

#define MAX_BONE_WEIGHTS 4


Bone MeshLoader::CreateBone(const std::string& name, int ID, const aiNodeAnim* channel)
{
	Bone bone;

	bone.m_NumPositions = channel->mNumPositionKeys;
	bone.m_Name = name;
	bone.m_ID = ID;

	for (int positionIndex = 0; positionIndex < bone.m_NumPositions; ++positionIndex)
	{
		aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
		float timeStamp = channel->mPositionKeys[positionIndex].mTime;
		KeyPosition data;
		data.position = aiToGlm(aiPosition);
		data.timeStamp = timeStamp;
		bone.m_Positions.push_back(data);
	}

	bone.m_NumRotations = channel->mNumRotationKeys;
	for (int rotationIndex = 0; rotationIndex < bone.m_NumRotations; ++rotationIndex)
	{
		aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
		float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
		KeyRotation data;
		data.orientation = aiToGlm(aiOrientation);
		data.timeStamp = timeStamp;
		bone.m_Rotations.push_back(data);
	}

	bone.m_NumScalings = channel->mNumScalingKeys;
	for (int keyIndex = 0; keyIndex < bone.m_NumScalings; ++keyIndex)
	{
		aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
		float timeStamp = channel->mScalingKeys[keyIndex].mTime;
		KeyScale data;
		data.scale = aiToGlm(scale);
		data.timeStamp = timeStamp;
		bone.m_Scales.push_back(data);
	}

	return bone;
}

void MeshLoader::ReadMissingBones(const aiAnimation* animation, SkeletalAnimation* skeletalAnim, std::unordered_map<std::string, BoneInfo>& boneMap, aiMesh* mesh)
{
	int size = animation->mNumChannels;

	auto& boneInfoMap = boneMap;//getting m_BoneInfoMap from Model class
	int boneCount = boneMap.size(); //getting the m_BoneCounter from Model class

	//reading channels(bones engaged in an animation and their keyframes)
	for (int i = 0; i < size; i++)
	{
		auto channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;

		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			boneInfoMap[boneName].id = boneCount;
			boneCount++;
		}
		skeletalAnim->m_Bones.push_back(CreateBone(channel->mNodeName.data,
			boneInfoMap[channel->mNodeName.data].id, channel));
	}

	for (int i = 0; i < mesh->mNumBones; i++)
	{
		std::string boneName = mesh->mBones[i]->mName.C_Str();
		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			boneInfoMap[boneName].id = boneCount;
			boneInfoMap[boneName].offset = aiToGlm(mesh->mBones[i]->mOffsetMatrix);
			boneCount++;
		}
	}
	skeletalAnim->m_BoneInfoMap = boneInfoMap;
}

void MeshLoader::ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src, std::unordered_map<std::string, BoneInfo>& boneMap)
{
	assert(src);

	dest.name = src->mName.data;
	dest.transformation = aiToGlm(src->mTransformation);
	dest.childrenCount = src->mNumChildren;


	for (int i = 0; i < src->mNumChildren; i++)
	{
		AssimpNodeData newData;
		ReadHeirarchyData(newData, src->mChildren[i],boneMap);
		dest.children.push_back(newData);
	}
}

std::tuple<std::shared_ptr<SkeletalAnimation>, std::shared_ptr<SA::SkeletalModel>> 
MeshLoader::LoadAnimation(const std::string& animationPath, std::unordered_map<std::string,BoneInfo>& boneMap )
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate |
		aiProcess_SortByPType | aiProcess_LimitBoneWeights |
		aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast);

	std::shared_ptr<SkeletalAnimation> skeletalAnim = std::make_shared<SkeletalAnimation>();

	if (scene->mNumAnimations == 0)
		return std::make_tuple(nullptr,nullptr);


	for (int iAnim = 0; iAnim < scene->mNumAnimations; iAnim++)
	{
		auto animation = scene->mAnimations[0];
		skeletalAnim->m_Duration = animation->mDuration;
		skeletalAnim->m_TicksPerSecond = animation->mTicksPerSecond;
		ReadHeirarchyData(skeletalAnim->m_RootNode, scene->mRootNode, boneMap);
		for (UINT i = 0; i < scene->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[i];
			ReadMissingBones(animation, skeletalAnim.get(), boneMap, mesh);
		}
	}
	
	std::shared_ptr<SA::SkeletalModel> saModel = std::make_shared<SA::SkeletalModel>();
	AssimpConverter::Convert(scene, *(saModel.get()));
	return std::make_tuple(skeletalAnim,saModel);
}

Matrix MeshLoader::aiToGlm(const aiMatrix4x4& v)
{
	Matrix out(v.a1, v.a2, v.a3, v.a4,
		v.b1, v.b2, v.b3, v.b4,
		v.c1, v.c2, v.c3, v.c4,
		v.d1, v.d2, v.d3, v.d4);
	return out.Transpose();
}





Vector3 MeshLoader::aiToGlm(const aiVector3D& v)
{
	Vector3 out(v.x, v.y, v.z);
	return out;
}



Quaternion MeshLoader::aiToGlm(const aiQuaternion& v)
{
	return Quaternion(v.x, v.y, v.z,v.w);
}


void MeshLoader::SetVertexBoneData(std::vector<Vector4>& vertices,int vertexID, int boneID, float weight)
{
	Vector4& boneIDs = vertices[vertexID * 7 + 5];
	Vector4& boneWeights = vertices[vertexID * 7 + 6];
	
	if (boneIDs.x < 0 && boneWeights.x < 0)
	{
		boneIDs.x = boneID;
		boneWeights.x = weight;
	}
	else if (boneIDs.y < 0 && boneWeights.y < 0)
	{
		boneIDs.y = boneID;
		boneWeights.y = weight;
	}
	else if (boneIDs.z < 0 && boneWeights.z < 0)
	{
		boneIDs.z = boneID;
		boneWeights.z = weight;
	}
	else if (boneIDs.w < 0 && boneWeights.w < 0)
	{
		boneIDs.w = boneID;
		boneWeights.w = weight;
	}
}

void MeshLoader::ExtractBoneWeightForVertices(std::vector<DirectX::SimpleMath::Vector4>& vertices, std::unordered_map<std::string,BoneInfo>& boneMap, aiMesh* mesh, const aiScene* scene)
{
	static aiScene* prevScene;
	static int boneCtr = 0;

	if (prevScene != scene)
	{
		scene = prevScene;
		boneCtr = 0;
	}

	for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
	{
		int boneID = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
		
		if (boneMap.find(boneName) == boneMap.end())
		{
			BoneInfo newBoneInfo;
			newBoneInfo.id = boneCtr;
			newBoneInfo.offset = aiToGlm(mesh->mBones[boneIndex]->mOffsetMatrix);
			boneMap[boneName] = newBoneInfo;
			boneID = boneCtr;
			boneCtr++;
		}
		else
		{
			boneID = boneMap[boneName].id;
		}

		auto weights = mesh->mBones[boneIndex]->mWeights;
		int numWeights = mesh->mBones[boneIndex]->mNumWeights;

		if (numWeights > 4)
			std::cout << " ";

		for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
		{
			int vertexId = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			assert(vertexId <= vertices.size());
			SetVertexBoneData(vertices,vertexId, boneID, weight);
		}
	}
	std::cout << std::endl;
}

std::shared_ptr<Mesh> MeshLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, std::unordered_map<std::string, BoneInfo>& m_BoneInfoMap)
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
		DirectX::XMFLOAT4 bonesIDs = DirectX::XMFLOAT4(-1, -1, -1, -1);
		DirectX::XMFLOAT4 boneWeights = DirectX::XMFLOAT4(-1, -1, -1, -1);

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
		vertices.push_back(bonesIDs);
		vertices.push_back(boneWeights);

	}

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	//std::map<std::string, BoneInfo> boneMap = {};

	ExtractBoneWeightForVertices(vertices,m_BoneInfoMap, mesh, scene);
	
	
	auto msh = std::make_shared<Mesh>(vertices, indices);
	
	return  msh;
}

void MeshLoader::ProcessNode(const std::string& modelPath, std::vector<std::shared_ptr<Mesh>>& meshes,
	std::unordered_map<std::string, BoneInfo>& m_BoneInfoMap, aiNode* node, const aiScene* scene)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(MeshLoader::ProcessMesh(mesh, scene,m_BoneInfoMap));
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(modelPath,meshes,m_BoneInfoMap, node->mChildren[i], scene);
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

void MeshLoader::LoadModel(const std::string& modelPath, std::vector<std::shared_ptr<Mesh>>& meshes, std::unordered_map<std::string, BoneInfo>& m_BoneInfoMap)
{

	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(modelPath, aiProcess_Triangulate | 
		aiProcess_SortByPType | aiProcess_LimitBoneWeights |
		aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast);

	if (pScene == nullptr)
		return;



	ProcessNode(modelPath,meshes,m_BoneInfoMap, pScene->mRootNode, pScene);
	
}

std::shared_ptr<Mesh> MeshLoader::LoadSimpleMesh(const std::string& modelPath)
{
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::unordered_map<std::string, BoneInfo> empty;
	LoadModel(modelPath, meshes,empty);
	return meshes.at(0);
}
