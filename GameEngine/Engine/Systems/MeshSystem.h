#pragma once
#include "ISystem.h"
#include "SimpleMath.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/cimport.h"
#include "../Components/Mesh.h"
#include "SpriteBatch.h"



struct MeshComponent;
struct HardwareContext;
struct EngineContext;

class MeshSystem : public ISystem
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	bool LoadModel(MeshComponent& mesh);
	void LoadTexture(MeshComponent& mesh);
	void ProcessNode(MeshComponent& meshComp, aiNode* node, const aiScene* scene);
	std::hash<std::string> hasher;
	HardwareContext* hc;
	EngineContext* ec;
};

