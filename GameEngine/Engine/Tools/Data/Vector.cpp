#include "SimpleMath.h"
#include "foundation/PxVec3.h"
#include "foundation/PxQuat.h"
#include "Vector.h"
SyVector3::SyVector3()
{
}

SyVector3::SyVector3(float x_, float y_, float z_)
{
	x = x_;
	y = y_;
	z = z_;
}

SyVector3::SyVector3(const SyVector3& other_)
{
	x = other_.x;
	y = other_.y;
	z = other_.z;
}

SyVector3::SyVector3(const DirectX::SimpleMath::Vector3& dxVector3_)
{
	x = dxVector3_.x;
	y = dxVector3_.y;
	z = dxVector3_.z;
}



SyVector3::SyVector3(const physx::PxVec3& pxVector3_)
{
	x = pxVector3_.x;
	y = pxVector3_.y;
	z = pxVector3_.z;
}

SyVector3::operator DirectX::SimpleMath::Vector3() const
{
	return DirectX::SimpleMath::Vector3(x, y, z);
}

SyVector3::operator physx::PxVec3() const
{
	return physx::PxVec3(x, y, z);
}

SyVector3& SyVector3::operator=(const DirectX::SimpleMath::Vector3& dxVector3_)
{
	x = dxVector3_.x;
	y = dxVector3_.y;
	z = dxVector3_.z;
	return *this;
}

SyVector3& SyVector3::operator+=(const SyVector3& other_)
{
	x += other_.x;
	y += other_.y;
	z += other_.z;
	return *this;
}

SyVector3& SyVector3::operator-=(const SyVector3& other_)
{
	x -= other_.x;
	y -= other_.y;
	z -= other_.z;
	return *this;
}

SyVector3& SyVector3::operator*=(float factor_)
{
	x *= factor_;
	y *= factor_;
	z *= factor_;
	return *this;
}

SyVector3 SyVector3::PxQuatToEuler(const physx::PxQuat& pxQuat)
{
	DirectX::SimpleMath::Quaternion q(pxQuat.x, pxQuat.y, pxQuat.z, pxQuat.w);
	auto v = q.ToEuler();
	return SyVector3(v);
}

physx::PxQuat SyVector3::EulerToPxQuat(const SyVector3& euler)
{
	auto q = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(euler);
	physx::PxQuat qq = {q.x, q.y, q.z, q.w};
	return qq;
}

//SyVector3 operator+(const SyVector3& left_, const SyVector3& right_)
//{
//	SyVector3 newVec(left_);
//	return newVec += right_;
//}
//
//SyVector3 operator-(const SyVector3& left_, const SyVector3& right_)
//{
//	SyVector3 newVec(left_);
//	return newVec -= right_;
//}
//
//SyVector3 operator*(const SyVector3& vec_, float factor_)
//{
//	SyVector3 newVec(vec_);
//	return newVec *= factor_;
//}