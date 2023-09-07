#pragma once
#include "../Components/Mesh.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/cimport.h"

class MeshLoader
{
public:
	static std::shared_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);
	static void ProcessNode(const std::string& modelPath, std::vector<std::shared_ptr<Mesh>>& mesh, aiNode* node, const aiScene* scene);
	static void LoadTexture(const std::string& meshPath, ID3D11SamplerState** samplerState,
		ID3D11ShaderResourceView** texture);
	static void LoadModel(const std::string& modelPath, std::vector<std::shared_ptr<Mesh>>& meshes);
	static std::shared_ptr<Mesh> LoadSimpleMesh(const std::string& modelPath);

};

