#pragma once
#include "../Tools/Data/Vector.h"

namespace mono
{
	struct ProxyTimeData
	{
		float TotalTime;
		float DeltaTime;
	};

	struct ProxyTransformComp
	{
		SyVector3 Position;
		SyVector3 Rotation;
		SyVector3 Scale;
	};

	struct ProxyMeshComp
	{
		std::string TexturePath;
		std::string ModelPath;
	};
}