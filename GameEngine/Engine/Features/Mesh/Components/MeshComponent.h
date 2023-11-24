#pragma once
#include "../Mesh.h"
#include "../../../Components/Model.h"
#include "SimpleMath.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/cimport.h"
#include "SpriteBatch.h"
#include "../../vendor/WICTextureLoader.h"
#include <boost/uuid/uuid.hpp>
#include "../../../Components/Material.h"
#include <iostream>
#include <wrl.h>
#include <vector>
#include <string>
#include <wtypes.h>
#include "../../../Serialization/Serializable.h"
#include "../../../Core/Tools/FlagBitmask.h"

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

	//----User vars----
	std::vector<boost::uuids::uuid> materialUUIDs = {};
	boost::uuids::uuid modelUUID;

	//----Engine vars----

	std::shared_ptr<Model> model = nullptr;
	std::vector<std::shared_ptr<Material>> materials = {};
	uint32_t flags = SyEMeshComponentFlags::MESH_RENDER;
	UINT strides[1] = { 80 };
	UINT offsets[1] = { 0 };
	uint32_t hashMaterialUuid = 0;
	uint32_t hashModelUuid = 0;

	size_t MonoHash = 0; // read/write only by mono sync system

	SER_COMP(MeshComponent,
		modelUUID,
		materialUUIDs,
		flags);
};

inline size_t hash_value(const MeshComponent& mesh)
{
	size_t hash = 0;
	boost::hash_combine(hash, mesh.modelUUID);
	boost::hash_combine(hash, mesh.materialUUIDs);
	return hash;
}