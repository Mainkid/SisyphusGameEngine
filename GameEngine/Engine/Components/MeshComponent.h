#pragma once
#include "Mesh.h"
#include "Model.h"
#include "SimpleMath.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/cimport.h"
#include "SpriteBatch.h"
#include "../../vendor/WICTextureLoader.h"
#include <boost/uuid/uuid.hpp>
#include "Material.h"
#include <iostream>
#include <wrl.h>
#include <vector>
#include <string>
#include <wtypes.h>
#include "../Serialization/Serializable.h"
#include "../Tools/Data/FlagBitmask.h"

using namespace DirectX::SimpleMath;

class EngineCore;

enum SyEMeshComponentFlags
{
	MESH_RENDER		= 1,		//check if you want this mesh to be rendered
	MESH_COLLIDER	= 1 << 1	//check if you want this mesh to be used in the TrimeshColliderComponent
};

DEFINE_BITWISE_OPERATORS(SyEMeshComponentFlags);

struct MeshComponent
{
	MeshComponent()
	{
		
	};
	MeshComponent(boost::uuids::uuid modelUUID, unsigned flags = SyEMeshComponentFlags::MESH_RENDER)
	{
		this->modelUUID = modelUUID;
		this->flags = flags;
	};
	//Material* material = nullptr;
	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture = nullptr;
	//Microsoft::WRL::ComPtr<ID3D11SamplerState> SamplerState = nullptr;
	std::shared_ptr<Model> model = nullptr;
	std::vector<boost::uuids::uuid> materialUUIDs = {};
	std::vector<std::shared_ptr<Material>> materials = {};
	boost::uuids::uuid modelUUID;
	uint32_t flags = SyEMeshComponentFlags::MESH_RENDER;
	UINT strides[1] = { 80 };
	UINT offsets[1] = { 0 };
	uint32_t hashMaterial = 0;
	uint32_t hashModel = 0;
	SER_COMP(MeshComponent,
		modelUUID,
		materialUUIDs,
		flags);
};

