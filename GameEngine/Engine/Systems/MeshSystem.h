#pragma once
#include "ISystem.h"
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

class MeshSystem : public ISystem
{
public:
	void Init() override;
	void Run() override;
	void Destroy() override;
private:
	boost::hash<boost::uuids::uuid> hasherModel;
	boost::hash<std::vector<boost::uuids::uuid>> hasherMaterial;
	HardwareContext* hc;
	EngineContext* ec;
	ResourceService* rs;
};

