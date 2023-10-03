#pragma once
#include "Mesh.h"
#include "SimpleMath.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/cimport.h"
#include "SpriteBatch.h"
#include "../../vendor/WICTextureLoader.h"
#include "Material.h"
#include <iostream>
#include <wrl.h>
#include <vector>
#include <string>
#include <wtypes.h>

using namespace DirectX::SimpleMath;

class EngineCore;

struct MeshComponent
{
	MeshComponent()
	{
		this->texturePath = "Engine/Assets/DefaultTexture.png";
		this->modelPath = "Engine/Assets/Cube.fbx";
	};
	MeshComponent(std::string& modelPath, std::string& texturePath)
	{
		this->texturePath = texturePath;
		this->modelPath = modelPath;
	};
	//Material* material = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState = nullptr;
	std::vector<std::shared_ptr<Mesh>> meshes = {};
	UINT strides[1] = { 80 };
	UINT offsets[1] = { 0 };
	std::string texturePath;
	std::string modelPath;
	uint32_t hash = 0;
	
};

