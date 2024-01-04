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
#include "../../Serialization/Serializer.hpp"
#include "../../Serialization/Serializable.h"

#include "../../../vendor/skeletalAnim/AssimpConverter/AssimpConverter.h"

struct AssimpNodeData
{
	DirectX::SimpleMath::Matrix transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
	
	SER_DATA(AssimpNodeData, transformation, name, childrenCount, children);
};

class SkeletalAnimation
{
public:
	SkeletalAnimation() = default;
	float m_Duration;
	int m_TicksPerSecond;
	std::vector<Bone> m_Bones;
	AssimpNodeData m_RootNode;
	std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;

	SER_DATA(SkeletalAnimation, m_Duration, m_TicksPerSecond, m_Bones, m_RootNode, m_BoneInfoMap);
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