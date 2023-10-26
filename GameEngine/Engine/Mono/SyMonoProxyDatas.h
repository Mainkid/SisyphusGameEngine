#pragma once

#include <string>

#include "../Tools/Data/Vector.h"

namespace mono
{
	struct ProxyVector3
	{
		float X = 0;
		float Y = 0;
		float Z = 0;

		ProxyVector3() { }
		ProxyVector3(const SyVector3& v) : X(v.x), Y(v.y), Z(v.z) { }

		operator SyVector3() const { return { X, Y, Z }; }
	};

	struct ProxyTimeData
	{
		float TotalTime;
		float DeltaTime;
	};

	struct ProxyTransformComp
	{
		ProxyVector3 Position;
		ProxyVector3 Rotation;
		ProxyVector3 Scale;
	};

	struct ProxyMeshComp
	{
		std::string TexturePath;
		std::string ModelPath;
	};
}