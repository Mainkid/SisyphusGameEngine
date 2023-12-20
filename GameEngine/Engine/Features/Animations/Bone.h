#pragma once
#include <SimpleMath.h>
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//#include "../Mesh/MeshLoader.h"

//#include "../../../vendor/skeletalAnim/AssimpConverter/AssimpConverter.h"

struct BoneInfo {
    int id;
    DirectX::SimpleMath::Matrix offset;
};

struct KeyPosition
{
	DirectX::SimpleMath::Vector3 position;
	float timeStamp;
};

struct KeyRotation
{
	DirectX::SimpleMath::Quaternion orientation;
	float timeStamp;
};

struct KeyScale
{
	DirectX::SimpleMath::Vector3 scale;
	float timeStamp;
};

class Bone
{
public:
	std::vector<KeyPosition> m_Positions;
	std::vector<KeyRotation> m_Rotations;
	std::vector<KeyScale> m_Scales;
	int m_NumPositions;
	int m_NumRotations;
	int m_NumScalings;

	DirectX::SimpleMath::Matrix m_LocalTransform;
	std::string m_Name;
	int m_ID;

};