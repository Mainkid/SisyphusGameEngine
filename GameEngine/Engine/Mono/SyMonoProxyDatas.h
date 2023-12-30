#pragma once

#include <string>
#include <mono/jit/jit.h>

#include "../Core/Tools/Structures/Vector.h"
#include "../Features/Lighting/Components/LightComponent.h"
#include "../Features/Physics/Components/ColliderComponent.h"
#include "../Features/Physics/Components/RBodyComponent.h"
#include "../Features/Particles/Components/ParticleComponent.h"

namespace mono
{
enum class EProxyCompId
{
	SceneObject,
	Transform,
	Mesh,
	Light,
	Collider,
	Rigid,
	Skybox,
	Particles
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
		case EProxyCompId::Skybox: return "Skybox";
		case EProxyCompId::Particles: return "Particles";
		default: return "Missing";
		}
	}
};


enum class EProxyResourceType
{
	Model,
	Material,
	Cubemap,
	Texture
};

struct ProxyVector2
{
	float X = 0;
	float Y = 0;

	ProxyVector2() {}
	ProxyVector2(float x, float y) : X(x), Y(y) { }
	ProxyVector2(const DirectX::SimpleMath::Vector2& v) : X(v.x), Y(v.y) { }
	ProxyVector2(const FloatPair& pair) : X(pair.f1), Y(pair.f2) {}

	operator DirectX::SimpleMath::Vector2() const { return { X, Y}; }
	operator FloatPair() const { return { X, Y}; }
};

struct ProxyVector3
{
	float X = 0;
	float Y = 0;
	float Z = 0;

	ProxyVector3() {}
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

struct ProxyVector4Pair
{
	ProxyVector4 A;
	ProxyVector4 B;

	ProxyVector4Pair() {}
	ProxyVector4Pair(const VectorPair& pair) : A(pair.vec1), B(pair.vec2) {}
	
	operator VectorPair() const { return { A, B }; }
};

struct ProxyCurve
{
	ProxyVector2 P1;
	ProxyVector2 P2;
	ProxyVector4 IsUsing;
	bool IsReversed;

	ProxyCurve() {}
	ProxyCurve(const UniformCurve& curve) :
		P1(curve.P1), P2(curve.P2),
		IsUsing(curve.IsUsing), IsReversed(curve.IsReversed) {}
		
	operator UniformCurve() const
	{
		return {
			{0, 0},
			P1,
			P2,
			{1, 1},
			IsUsing,
			IsReversed
		};
	}
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

struct ProxySkyboxComp
{
	MonoString* CubemapUuid;
};

struct ProxyParticlesComp
{
	struct InputData
	{
		float F;                                     
		ProxyVector2 RandomBetweenConstsF;
		ProxyCurve Curve;
		ProxyVector4 V4;
		ProxyVector4Pair RandomBetweenConstsV;
		EInputType InputType;

		InputData() {}
		InputData(const ParticleInputDataF& data) :
			F(data.Fvalue), RandomBetweenConstsF(data.RandomBetweenConstsF),
			Curve(data.Curve),
			V4(data.V4value), RandomBetweenConstsV(data.RandomBetweenConstsV),
			InputType(data.InputType) {}
		
		operator ParticleInputDataF() const
		{
			return
			{
				F,
				RandomBetweenConstsF,
				Curve,
				V4,
				RandomBetweenConstsV,
				InputType
			};
		}
	};

	struct BurstData
	{
		float Time;                                    
		float TimeSinceLastBurst;
		InputData Count;                       
		bool IsLooping;
		float Probability;

		BurstData() {}
		BurstData(const ParticleBurst& data) :
			Time(data.Time), TimeSinceLastBurst(data.TimeSinceLastBurst),
			Count(data.Count), IsLooping(data.IsLooping), Probability(data.Probability){}

		operator ParticleBurst() const
		{
			return
			{
				Time,
				TimeSinceLastBurst,
				Count,
				IsLooping,
				Probability
			};
		}
	};
	
	float Duration;                                  
	bool IsLooping;                                
	InputData StartDelayTime;              
	InputData StartLifeTime;             
	InputData StartSpeed;                 
	InputData StartSize;                     
	InputData StartColor;                  
	InputData StartRotation;				  
	InputData SizeOverLifetime;				
	InputData SpeedOverLifetime;					
	InputData RotationOverLifetime;		  
	uint32_t MaxParticles;
	bool IsLit;
	float AmbientAmount;
	
	InputData RateOverTime;
	BurstData* Bursts;
	int BurstsCount;
	
	EParticleEmitShape ParticleEmitShape;
	float Angle;
	float Radius;

	MonoString* TextureUuid;

	bool IsDirty;
};
}