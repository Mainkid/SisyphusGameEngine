#pragma once

#include <boost/container_hash/hash.hpp>

#include "SimpleMath.h"
#include "foundation/PxVec3.h"
#include "foundation/PxQuat.h"

struct SyVector3
{
	float& x;
	float& y;
	float& z;

private:
	union
	{
		DirectX::SimpleMath::Vector3 dxVector3;
		physx::PxVec3 pxVector3;
	};
public:
	SyVector3() : dxVector3(), x(dxVector3.x), y(dxVector3.y), z(dxVector3.z) {};
	SyVector3(float x_, float y_, float z_) : SyVector3()
	{
		x = x_;
		y = y_;
		z = z_;
	};
	SyVector3(const SyVector3& other_) : SyVector3()
	{
		x = other_.x;
		y = other_.y;
		z = other_.z;
	};
	SyVector3(const DirectX::SimpleMath::Vector3& dxVector3_) : SyVector3()
	{
		x = dxVector3_.x;
		y = dxVector3_.y;
		z = dxVector3_.z;
	};
	SyVector3(const physx::PxVec3& pxVector3_) : SyVector3()
	{
		x = pxVector3_.x;
		y = pxVector3_.y;
		z = pxVector3_.z;
	}
	operator const DirectX::SimpleMath::Vector3&() const
	{
		return dxVector3;
	}
	operator const DirectX::XMVECTOR&() const
	{
		return dxVector3;
	}
	operator const physx::PxVec3& () const
	{
		return pxVector3;
	}
	operator DirectX::SimpleMath::Vector3&()
	{
		return dxVector3;
	}
	operator DirectX::XMVECTOR()&
	{
		return dxVector3;
	}
	operator physx::PxVec3& ()
	{
		return pxVector3;
	};
	SyVector3& operator= (const SyVector3& dxVector3_)
	{
		x = dxVector3_.x;
		y = dxVector3_.y;
		z = dxVector3_.z;
		return *this;
	}
	SyVector3& operator+= (const SyVector3& other_)
	{
		x += other_.x;
		y += other_.y;
		z += other_.z;
		return *this;
	}
	SyVector3& operator-= (const SyVector3& other_)
	{
		x -= other_.x;
		y -= other_.y;
		z -= other_.z;
		return *this;
	}
	SyVector3& operator*= (float factor_)
	{
		x *= factor_;
		y *= factor_;
		z *= factor_;
		return *this;
	}
	SyVector3& operator/= (float dividor_)
	{
		x /= dividor_;
		y /= dividor_;
		z /= dividor_;
		return *this;
	}
	static SyVector3 PxQuatToEuler(const physx::PxQuat& pxQuat) //returns euler angle vector
	{
		DirectX::SimpleMath::Quaternion q(pxQuat.x, pxQuat.y, pxQuat.z, pxQuat.w);
		auto v = q.ToEuler();
		return SyVector3(v);
	}
	static physx::PxQuat EulerToPxQuat(const SyVector3& euler)
	{
		auto q = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(euler);
		physx::PxQuat qq = { q.x, q.y, q.z, q.w };
		return qq;
	}

	
};

inline std::size_t hash_value(SyVector3 const& vec)
{
	size_t seed = 0;
	boost::hash_combine(seed, vec.x);
	boost::hash_combine(seed, vec.y);
	boost::hash_combine(seed, vec.z);
	return seed;
}