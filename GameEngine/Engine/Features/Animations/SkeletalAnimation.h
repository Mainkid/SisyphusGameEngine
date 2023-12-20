#pragma once
#include <SimpleMath.h>
#include <vector>
#include <map>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Bone.h"
#include <memory>

#include "../../../vendor/skeletalAnim/AssimpConverter/AssimpConverter.h"

struct AssimpNodeData
{
	DirectX::SimpleMath::Matrix transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

class SkeletalAnimation
{
public:
	SkeletalAnimation() = default;
	float m_Duration;
	int m_TicksPerSecond;
	std::vector<Bone> m_Bones;
	AssimpNodeData m_RootNode;
	std::map<std::string, BoneInfo> m_BoneInfoMap;

};

class SkeletalAnimator
{
public:

	SkeletalAnimator()
	{
		m_CurrentTime = 0.0;
		m_FinalBoneMatrices.reserve(100);

		for (int i = 0; i < 100; i++)
			m_FinalBoneMatrices.push_back(DirectX::SimpleMath::Matrix::Identity);
	}

	std::vector<DirectX::SimpleMath::Matrix> m_FinalBoneMatrices;
	std::shared_ptr<SkeletalAnimation> m_CurrentAnimation;
	float m_CurrentTime;
	float m_DeltaTime;
};