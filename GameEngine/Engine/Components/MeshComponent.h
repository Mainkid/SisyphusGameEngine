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

using namespace DirectX::SimpleMath;

class EngineCore;

struct MeshComponent
{
	MeshComponent()
	{
		
	};
	MeshComponent(boost::uuids::uuid modelUUID)
	{
		this->modelUUID = modelUUID;
	};

	//----User vars----
	std::vector<boost::uuids::uuid> materialUUIDs = {};
	boost::uuids::uuid modelUUID;

	//----Engine vars----

	std::shared_ptr<Model> model = nullptr;
	std::vector<std::shared_ptr<Material>> materials = {};
	UINT strides[1] = { 80 };
	UINT offsets[1] = { 0 };
	uint32_t hashMaterial = 0;
	uint32_t hashModel = 0;
	SER_COMP(MeshComponent,
		modelUUID,
		materialUUIDs)
};

