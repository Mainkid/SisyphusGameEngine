#include "SimpleMath.h"
#include "foundation/PxVec3.h"
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

SyVector3 SyVector3::operator=(const DirectX::SimpleMath::Vector3& dxVector3_)
{
	x = dxVector3_.x;
	y = dxVector3_.y;
	z = dxVector3_.z;
	return *this;
}

void FromQuatToEuler(const SyVector3& quat, SyVector3& res)
{
	auto q = DirectX::SimpleMath::Quaternion::Quaternion((DirectX::SimpleMath::Vector3)quat);
	res = q.DirectX::SimpleMath::Quaternion::ToEuler();
}
