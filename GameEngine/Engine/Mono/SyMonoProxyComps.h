#pragma once
#include "../Tools/Data/Vector.h"

namespace mono
{
	struct SyMonoProxyTransformComp
	{
		SyVector3 Position;
		SyVector3 Rotation;
		SyVector3 Scale;
	};

	struct SyMonoProxyMeshComp
	{
		std::string TexturePath;
		std::string ModelPath;
	};
}