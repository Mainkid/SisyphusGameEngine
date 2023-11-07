#pragma once

#include "Serializable.h"
#include "../Tools/Data/Vector.h"
#include "SimpleMath.h"
#include <boost/uuid/uuid.hpp>

namespace DirectX
{
	namespace SimpleMath
	{
		SER_DATA_OUTER(DirectX::SimpleMath::Vector4, x, y, z, w)
		SER_DATA_OUTER(DirectX::SimpleMath::Vector3, x, y, z)
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

