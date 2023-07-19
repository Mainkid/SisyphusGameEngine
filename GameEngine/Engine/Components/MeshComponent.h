#pragma once
#include "GameComponent.h"

#include "Mesh.h"
#include "../DirectXSDK/SimpleMath.h"
#include <assimp/postprocess.h>
#include "../DirectXSDK/assimp/scene.h"
#include "../DirectXSDK/assimp/Importer.hpp"
#include "../DirectXSDK/assimp/cimport.h"
#include "../DirectXSDK/SpriteBatch.h"
#include "../DirectXSDK/WICTextureLoader.h"
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

