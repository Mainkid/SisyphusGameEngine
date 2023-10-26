#pragma once
#include "ResourceBase.h"
#include <vector>
#include <memory>
#include "Mesh.h"

struct Model : public ResourceBase
{
	std::vector<std::shared_ptr<Mesh>> meshes = {};
};