#pragma once

#include <string>
#include <mono/jit/jit.h>

#include "../../Core/Tools/Structures/Vector.h"
#include "../../Features/Particles/Components/ParticleComponent.h"

namespace mono
{
enum class EProxyResourceType
{
	Model,
	Material,
	Cubemap,
	Texture,
	Sound,
	Animation
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
}