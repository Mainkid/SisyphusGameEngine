#pragma once
#include "../Features/Resources/ResourceBase.h"
#include <vector>
#include <memory>
#include "../Features/Mesh/Mesh.h"

struct Model : public ResourceBase
{
	std::vector<std::shared_ptr<Mesh>> meshes = {};
};