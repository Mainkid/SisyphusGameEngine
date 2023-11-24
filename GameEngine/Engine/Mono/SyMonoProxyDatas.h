#pragma once

#include <string>
#include <mono/jit/jit.h>

#include "../Core/Tools/Structures/Vector.h"

namespace mono
{
	enum class EProxyCompId
	{
		Transform,
		Mesh,
		Light,
		Rigid
	};

	class ProxyCompIdExt
	{
	public:
		static constexpr std::string ToStr(EProxyCompId id)
		{
			switch(id)
			{
			case EProxyCompId::Transform: return "Transform";
			case EProxyCompId::Mesh: return "Mesh";
			case EProxyCompId::Light: return "Light";
			case EProxyCompId::Rigid: return "Rigid";
			default: return "Missing";
			}
		}
	};


	enum class EProxyResourceType
	{
		Model,
		Material
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
		MonoString* ModelUuid;
		MonoArray* MaterialsUuids;
	};

	struct ProxyLightComp
	{
		ELightType LightType;
		LightBehavior LightBehavior;
		ProxyVector4 Color;
		ProxyVector4 ParamsRadiusAndAttenuation;
	};


}