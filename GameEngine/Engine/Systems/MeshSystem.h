#pragma once
#include "ISystem.h"
#include "SimpleMath.h"
#include "../Core/EngineCore.h"

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

