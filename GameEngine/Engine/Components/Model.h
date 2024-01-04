#pragma once
#include "../Features/Resources/ResourceBase.h"
#include <vector>
#include <memory>
#include <map>
#include <unordered_map>
#include "../Features/Mesh/Mesh.h"
#include "../Features/Animations/Bone.h"
#include "../Features/Animations/SkeletalAnimation.h"
#include <SkeletalAnimation/SkeletalModel.h>

struct Model : public ResourceBase
{
	std::vector<std::shared_ptr<Mesh>> meshes = {};
	std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;
};