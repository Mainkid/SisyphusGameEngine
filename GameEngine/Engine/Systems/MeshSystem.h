#pragma once
#include "ISystem.h"
#include "SimpleMath.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/cimport.h"
#include "../Components/Mesh.h"
#include "SpriteBatch.h"



class MeshComponent;

class MeshSystem : public ISystem
{
public:
	void Init() override;
	void Run() override;
	void Destroy() override;
private:
	bool LoadModel(MeshComponent& mesh);
	void LoadTexture(MeshComponent& mesh);
	void ProcessNode(MeshComponent& meshComp, aiNode* node, const aiScene* scene);
	std::shared_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);
	
	
	std::hash<std::string> hasher;
};

