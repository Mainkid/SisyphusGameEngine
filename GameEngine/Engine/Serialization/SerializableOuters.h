#pragma once

#include "Serializable.h"
#include "../Tools/Data/Vector.h"
#include "SimpleMath.h"

namespace DirectX
{
	namespace SimpleMath
	{
		SER_DATA_OUTER(DirectX::SimpleMath::Vector4, x, y, z, w)
		
	}
}

SER_DATA_OUTER(SyVector3, x, y, z)