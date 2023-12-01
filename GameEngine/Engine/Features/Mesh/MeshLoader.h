#pragma once
#include "Mesh.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/material.h"
#include "assimp/Importer.hpp"
#include "assimp/cimport.h"
#include "../../vendor/WICTextureLoader.h"
#include "../../Core/Tools/ErrorLogger.h"

class MeshLoader
{
public:
	static std::shared_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);
	static void ProcessNode(const std::string& modelPath, std::vector<std::shared_ptr<Mesh>>& mesh, aiNode* node, const aiScene* scene);
	static SyResult LoadTexture(const std::string& meshPath, ID3D11SamplerState** samplerState,
		ID3D11ShaderResourceView** texture,bool isSRGB=false);
	static void LoadModel(const std::string& modelPath, std::vector<std::shared_ptr<Mesh>>& meshes);
	static std::shared_ptr<Mesh> LoadSimpleMesh(const std::string& modelPath);
	static void GenerateTangentAndBitangent(
		std::vector<DirectX::SimpleMath::Vector4>& vertices,
		std::vector<DirectX::SimpleMath::Vector4>& uvs,
		std::vector<DirectX::SimpleMath::Vector4>& tangent,
		std::vector<DirectX::SimpleMath::Vector4>& bitangent);

};

