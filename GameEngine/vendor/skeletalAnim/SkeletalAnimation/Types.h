#pragma once

// GLM

// STD
#include <vector>
#include <string>
#include <SimpleMath.h>

namespace SA
{
	struct sWeight
	{
		unsigned int VertexID;
		float Weight;
	};

	struct sBone
	{
		std::string Name;

		DirectX::SimpleMath::Matrix NodeTransform;
		DirectX::SimpleMath::Matrix OffsetMatrix; // T-Pose to local bone space
		DirectX::SimpleMath::Matrix FinalTransformation;

		unsigned int NumWeights;
		sWeight* pWeights;

		unsigned int NumChildren;
		unsigned int* pChildren;
	};

	struct sAnimatedMesh
	{
		unsigned int NumVertices;

		DirectX::SimpleMath::Vector3* pVertices;
		DirectX::SimpleMath::Vector3* pNormals;

		DirectX::SimpleMath::Vector3* pTransformedVertices;
		DirectX::SimpleMath::Vector3* pTransformedNormals;

		unsigned int NumIndices;
		unsigned int* pIndices;
	};

	struct sSkeleton
	{
		std::vector<sBone> Bones;
	};

	template <typename _Ty>
	struct sNodeAnimationKey
	{
		_Ty Value;
		float Time;
	};

	struct sNodeAnimation
	{
		std::string Name;

		std::vector<sNodeAnimationKey<DirectX::SimpleMath::Vector3> > PositionKeys;
		std::vector<sNodeAnimationKey<DirectX::SimpleMath::Quaternion> > RotationKeys;
	};

	struct sAnimation
	{
		std::vector<sNodeAnimation> NodeAnimations;

		float TicksPerSecond;
		float Duration;
	};
}