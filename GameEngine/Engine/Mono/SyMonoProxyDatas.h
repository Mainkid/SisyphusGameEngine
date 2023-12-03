#pragma once

#include <string>
#include <mono/jit/jit.h>

#include "../Core/Tools/Structures/Vector.h"
#include "../Features/Lighting/Components/LightComponent.h"
#include "../Features/Physics/Components/ColliderComponent.h"
#include "../Features/Physics/Components/RBodyComponent.h"

namespace mono
{
	enum class EProxyCompId
	{
		SceneObject,
		Transform,
		Mesh,
		Light,
		Collider,
		Rigid
	};

	class ProxyCompIdExt
	{
	public:
		static constexpr std::string ToStr(EProxyCompId id)
		{
			switch(id)
			{
			case EProxyCompId::SceneObject: return "SceneObject";
			case EProxyCompId::Transform: return "Transform";
			case EProxyCompId::Mesh: return "Mesh";
			case EProxyCompId::Light: return "Light";
			case EProxyCompId::Collider: return "Collider";
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

	struct ProxySceneObjectComp
	{
		MonoString* Name;
		bool IsActive;
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
		float PointLightRadius;
		bool ShouldCastShadows;
	};

	struct ProxyColliderComp
	{
		SyEPrimitiveColliderType Type;
		ProxyVector3 Extent;
		float Radius;
		float HalfHeight;
	};

	struct ProxyRigidComp
	{
		SyERBodyType Type;
		float Mass;

		bool IsAutoMass;
		bool IsKinematic;
		bool IsGravityOn;

		ProxyVector3 LinearVelocity;
		ProxyVector3 AngularVelocity;
	};
}