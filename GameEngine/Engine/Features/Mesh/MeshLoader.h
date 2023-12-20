#pragma once
#include "Mesh.h"
#include <map>
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/material.h"
#include "assimp/Importer.hpp"
#include "assimp/cimport.h"
#include "../../vendor/WICTextureLoader.h"
#include "../../Core/Tools/ErrorLogger.h"
#include "../Animations/Bone.h"
#include "../Animations/SkeletalAnimation.h"
#include <SkeletalAnimation/SkeletalModel.h>

class MeshLoader
{
public:

	static Bone CreateBone(const std::string& name, int ID, const aiNodeAnim* channel);
	static void ReadMissingBones(const aiAnimation* animation, SkeletalAnimation* skeletalAnim, std::map<std::string, BoneInfo>& boneMap, aiMesh* mesh);
	static void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src, std::map<std::string, BoneInfo>& boneMap);
	static std::shared_ptr<SkeletalAnimation> LoadAnimation(const std::string& animationPath, std::map<std::string, BoneInfo>& boneMap);

	static DirectX::SimpleMath::Matrix aiToGlm(const aiMatrix4x4& v);

	static  DirectX::SimpleMath::Vector3 aiToGlm(const aiVector3D& v);

	static DirectX::SimpleMath::Quaternion aiToGlm(const aiQuaternion& v);

	static DirectX::SimpleMath::Matrix AssimpToMatrix(aiMatrix4x4& mat);
	static void SetVertexBoneData(std::vector<DirectX::SimpleMath::Vector4>& vertices, int vertexID, int boneID, float weight);
	static void ExtractBoneWeightForVertices(std::vector<DirectX::SimpleMath::Vector4>& vertices, std::map<std::string, BoneInfo>& boneMap, aiMesh* mesh, const aiScene* scene);

	static std::shared_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);
	static void ProcessNode(const std::string& modelPath, std::vector<std::shared_ptr<Mesh>>& mesh, std::map<std::string, BoneInfo>& m_BoneInfoMap,
		aiNode* node, const aiScene* scene);
	static SyResult LoadTexture(const std::string& meshPath, ID3D11SamplerState** samplerState,
		ID3D11ShaderResourceView** texture,bool isSRGB=false);
	static void LoadModel(const std::string& modelPath, std::vector<std::shared_ptr<Mesh>>& meshes, std::map<std::string, BoneInfo>& m_BoneInfoMap);
	static std::shared_ptr<Mesh> LoadSimpleMesh(const std::string& modelPath);
	static void GenerateTangentAndBitangent(
		std::vector<DirectX::SimpleMath::Vector4>& vertices,
		std::vector<DirectX::SimpleMath::Vector4>& uvs,
		std::vector<DirectX::SimpleMath::Vector4>& tangent,
		std::vector<DirectX::SimpleMath::Vector4>& bitangent);

};

