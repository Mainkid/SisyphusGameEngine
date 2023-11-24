#pragma once
#include "../Core/ECS/SystemBase.h"
#include "SimpleMath.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/cimport.h"
#include "../Components/Mesh.h"
#include "SpriteBatch.h"
#include <boost/uuid/uuid.hpp>
#include <boost/functional/hash.hpp>


struct MeshComponent;
struct HardwareContext;
struct EngineContext;
class ResourceService;

class MeshSystem : public SystemBase
{
public:
	SyResult Init() override;
	SyResult Run() override;
	SyResult Destroy() override;
private:
	boost::hash<boost::uuids::uuid> hasherModelUuid;
	boost::hash<std::vector<boost::uuids::uuid>> hasherMaterialsUUids;
	HardwareContext* hc;
	EngineContext* ec;
	ResourceService* rs;
};

