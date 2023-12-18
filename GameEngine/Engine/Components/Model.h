#pragma once
#include "../Features/Resources/ResourceBase.h"
#include <vector>
#include <memory>
#include "../Features/Mesh/Mesh.h"
#include <SkeletalAnimation/SkeletalModel.h>

struct Model : public ResourceBase
{
	std::vector<std::shared_ptr<Mesh>> meshes = {};
	std::shared_ptr<SA::SkeletalModel> skeleton = nullptr;
};