#pragma once
#include "GameComponent.h"

#include "Mesh.h"
#include "../../vendor/SimpleMath.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/cimport.h"
#include "../../vendor/SpriteBatch.h"
#include "../../vendor/WICTextureLoader.h"
#include <iostream>
#include <wrl.h>
#include <vector>
#include <string>
#include <wtypes.h>

using namespace DirectX::SimpleMath;

class EngineCore;

class MeshComponent
{
public:
	MeshComponent();
	MeshComponent(std::string& modelPath, LPCWSTR& texturePath);
	~MeshComponent() = default;
	bool LoadModel();
	void LoadTexture();
	void UpdateMesh(std::string modelPath= "Engine/Assets/trash2.obj", LPCWSTR texturePath= L"Engine/Assets/DefaultTexture.png");
	void ProcessNode(aiNode* node, const aiScene* scene);
	std::shared_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);

	void Initialize();

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	std::vector<std::shared_ptr<Mesh>> meshes = {};

	UINT strides[1] = { 48 };
	UINT offsets[1] = { 0 };

private:
	
	LPCWSTR texturePath;
	std::string modelPath;
	
};

