#pragma once

#include "SerializableMacros.h"
#include "../Core/Tools/Structures/Vector.h"
#include "SimpleMath.h"
#include <boost/uuid/uuid.hpp>

namespace DirectX
{
	namespace SimpleMath
	{
		SER_DATA_OUTER(DirectX::SimpleMath::Vector4, x, y, z, w)
		SER_DATA_OUTER(DirectX::SimpleMath::Vector3, x, y, z)
		SER_DATA_OUTER(DirectX::SimpleMath::Vector2, x, y)

		
		SER_DATA_OUTER(DirectX::SimpleMath::Matrix, _11, _12, _13, _14,
			 _21, _22, _23, _24,
			 _31, _32, _33, _34,
			 _41, _42, _43, _44)

		SER_DATA_OUTER(DirectX::SimpleMath::Quaternion, x,y,z,w )
	}
}

SER_DATA_OUTER(SyVector3, x, y, z)

namespace boost
{
	namespace uuids
	{
		
		SER_DATA_OUTER(boost::uuids::uuid, data)
	}
}


