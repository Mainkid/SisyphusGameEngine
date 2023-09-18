#pragma once
namespace DirectX
{
	namespace SimpleMath
	{
		struct Vector3;
	}
}

namespace physx
{
	template <class Type>
	class PxVec3T;
	typedef PxVec3T<float>	PxVec3;
}

struct SyVector3
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	SyVector3();
	SyVector3(float x_, float y_, float z_);
	SyVector3(const DirectX::SimpleMath::Vector3& dxVector3);
	SyVector3(const physx::PxVec3& pxVector3);
	operator DirectX::SimpleMath::Vector3() const;
	operator physx::PxVec3() const;
};

