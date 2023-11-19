#pragma once

#include <string>

#include "../Tools/Data/Vector.h"
#include "../Core/Rendering/Lights/ELightType.h"

namespace mono
{
	enum class ProxyCompId
	{
		Transform,
		Mesh,
		Light,
		Rigid
	};

	class ProxyCompIdExt
	{
	public:
		static constexpr std::string ToStr(ProxyCompId id)
		{
			switch(id)
			{
			case ProxyCompId::Transform: return "Transform";
			case ProxyCompId::Mesh: return "Mesh";
			case ProxyCompId::Light: return "Light";
			case ProxyCompId::Rigid: return "Rigid";
			default: return "Missing";
			}
		}
	};

	struct ProxyVector3
	{
		float X = 0;
		float Y = 0;
		float Z = 0;

		ProxyVector3() { }
		ProxyVector3(float x, float y, float z) : X(x), Y(y), Z(z) { }
		ProxyVector3(const SyVector3& v) : X(v.x), Y(v.y), Z(v.z) { }

		operator SyVector3() const { return { X, Y, Z }; }
	};

	struct ProxyVector4
	{
		float X = 0;
		float Y = 0;
		float Z = 0;
		float W = 0;

		ProxyVector4() {}
		ProxyVector4(float x, float y, float z, float w) : X(x), Y(y), Z(z), W(w) {}
		ProxyVector4(const DirectX::SimpleMath::Vector4& v) : X(v.x), Y(v.y), Z(v.z), W(v.w) {}

		operator DirectX::SimpleMath::Vector4() const { return { X, Y, Z, W }; }
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
		ProxyVector3 LocalPosition;
		ProxyVector3 LocalRotation;
		ProxyVector3 LocalScale;

		bool HasParent;
		uint32_t ParentEngineEnt;
	};

	struct ProxyMeshComp
	{
		std::string TexturePath;
		std::string ModelPath;
	};

	struct ProxyLightComp
	{
		ELightType LightType;
		LightBehavior LightBehavior;
		ProxyVector4 Color;
		ProxyVector4 ParamsRadiusAndAttenuation;
	};
}