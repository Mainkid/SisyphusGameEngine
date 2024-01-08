#pragma once

#include <boost/container_hash/hash.hpp>

#include "SimpleMath.h"
#include "../HashHelper.h"
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
	SyVector3(const float* vectorf) : SyVector3()
	{
		x = vectorf[0];
		y = vectorf[1];
		z = vectorf[2];
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
	SyVector3& operator/= (float divisor_)
	{
		x /= divisor_;
		y /= divisor_;
		z /= divisor_;
		return *this;
	}
	SyVector3 ReflectX() const
	{
		SyVector3 newVector = *this;
		newVector.x *= -1;
		return newVector;
	}
	SyVector3 ReflectY() const
	{
		SyVector3 newVector = *this;
		newVector.y *= -1;
		return newVector;
	}
	SyVector3 ReflectZ() const
	{
		SyVector3 newVector = *this;
		newVector.z *= -1;
		return newVector;
	}
	float Length() const
	{
		return x * x + y * y + z * z;
	}
	void Translate(const SyVector3& translation)
	{
		x += translation.x;
		y += translation.y;
		z += translation.z;
	}
	//rotate vector by yaw, pitch, roll angles	
	void Rotate(const SyVector3& eulerRotation)
	{
		DirectX::SimpleMath::Vector3::Transform(*this, DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(eulerRotation));
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

	friend bool operator== (const SyVector3& left, const SyVector3& right);
	friend SyVector3 operator+ (const SyVector3& left, const SyVector3& right);
	friend SyVector3 operator- (const SyVector3& left, const SyVector3& right);
	static const SyVector3 ZERO;
	static const SyVector3 ONE;
};

inline bool operator==(const SyVector3& left, const SyVector3& right)
{
	return (left.x == right.x) && (left.y == right.y) && (left.z == right.z);
}

inline SyVector3 operator+ (const SyVector3& left, const SyVector3& right)
{
	SyVector3 result = left;
	result += right;
	return result;
}
inline SyVector3 operator- (const SyVector3& left, const SyVector3& right)
{
	SyVector3 result = left;
	result -= right;
	return result;
}

inline std::size_t hash_value(SyVector3 const& vec)
{
	size_t seed = 0;
	boost::hash_combine(seed, vec.x);
	boost::hash_combine(seed, vec.y);
	boost::hash_combine(seed, vec.z);
	return seed;
}