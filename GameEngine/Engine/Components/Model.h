#pragma once
#include "../Features/Resources/ResourceBase.h"
#include <vector>
#include <memory>
#include <map>
#include "../Features/Mesh/Mesh.h"
#include "../Features/Animations/Bone.h"
#include "../Features/Animations/SkeletalAnimation.h"
#include <SkeletalAnimation/SkeletalModel.h>

struct Model : public ResourceBase
{
	std::vector<std::shared_ptr<Mesh>> meshes = {};
	std::shared_ptr<SA::SkeletalModel> skeletalAnim;
	std::map<std::string, BoneInfo> m_BoneInfoMap;
	std::shared_ptr<SkeletalAnimator> animator;
};