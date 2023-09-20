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
	template <class Type>
	class PxQuatT;
	typedef PxQuatT<float> PxQuat;
}

struct SyVector3
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	SyVector3();
	SyVector3(float x_, float y_, float z_);
	SyVector3(const DirectX::SimpleMath::Vector3& dxVector3_);
	SyVector3(const physx::PxVec3& pxVector3_);
	operator DirectX::SimpleMath::Vector3() const;
	operator physx::PxVec3() const;
	SyVector3 operator= (const DirectX::SimpleMath::Vector3& dxVector3_);

	static SyVector3 PxQuatToEuler(const physx::PxQuat& pxQuat); //returns euler angle vector
	static physx::PxQuat EulerToPxQuat(const SyVector3& euler);
};

