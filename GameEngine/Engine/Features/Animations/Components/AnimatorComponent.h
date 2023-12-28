#pragma once
#include <vector>
#include <memory>
#include "../SkeletalAnimation.h"
#include <boost/uuid/uuid.hpp>

class AnimatorComponent
{
public:
	AnimatorComponent()
	{
		m_CurrentTime = 0.0f;
		m_DeltaTime = 0.0f;
		m_FinalBoneMatrices.reserve(100);

		for (int i = 0; i < 100; i++)
			m_FinalBoneMatrices.push_back(DirectX::SimpleMath::Matrix::Identity);
	}

	uint32_t hash = 0;
	boost::uuids::uuid animationUUID = boost::uuids::nil_uuid();
	std::vector<DirectX::SimpleMath::Matrix> m_FinalBoneMatrices;
	std::shared_ptr<SkeletalAnimation> m_CurrentAnimation;
	float m_CurrentTime;
	float m_DeltaTime;
};