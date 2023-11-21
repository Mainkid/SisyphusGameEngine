#pragma once
#include "SimpleMath.h"
#include "../../vendor/entt/entt.hpp"
#include "../Tools/Data/Vector.h"
#include "../Serialization/Serializable.h"

using namespace DirectX::SimpleMath;
struct TransformComponent
{
	TransformComponent() = default;
	TransformComponent(Vector3 _pos, Vector3 _rot = Vector3::Zero, Vector3 _scale = Vector3(1, 1, 1))
	{
		this->_position = _pos;
		this->_rotation = _rot;
		this->scale = _scale;
	}
	~TransformComponent()
	{
	}

	SyVector3 _position = SyVector3(0, 0, 0);
	SyVector3 _rotation = SyVector3(0, 0, 0);
	SyVector3 scale = SyVector3(1, 1, 1);
	SyVector3 localPosition = SyVector3(0, 0, 0);
	SyVector3 localRotation = SyVector3(0, 0, 0);
	SyVector3 localScale = SyVector3(1, 1, 1);
	Matrix transformMatrix = Matrix::Identity;
	size_t worldHash = 0;
	size_t localHash = 0;
	uint32_t parent = entt::null;
	std::set<entt::entity> children = {};


	SER_COMP(TransformComponent,
		_position,
		_rotation,
		scale,
		localPosition,
		localRotation,
		localScale,
		parent)
};

namespace std
{
	template<> struct hash<SyVector3>
	{
		using argument_type = SyVector3;
		using result_type = std::size_t;
		result_type operator()(argument_type const& a) const
		{
			result_type const h1(std::hash<float>()(a.x));
			result_type const h2(std::hash<float>()(a.y));
			result_type const h3(std::hash<float>()(a.z));
			return h1 * 37 + (h1 * 37 + h2) * 37 +
				((h1 * 37 + h2) * 37 + h3) * 37;
		}
	};
}

namespace std
{
	template<> struct hash<Vector3>
	{
		using argument_type = Vector3;
		using result_type = std::size_t;
		result_type operator()(argument_type const& a) const
		{
			result_type const h1(std::hash<float>()(a.x));
			result_type const h2(std::hash<float>()(a.y));
			result_type const h3(std::hash<float>()(a.z));
			return h1 * 37 + (h1 * 37 + h2) * 37 +
				((h1 * 37 + h2) * 37 + h3) * 37;
		}
	};
}

namespace std
{
	template<> struct hash<TransformComponent>
	{
		using argument_type = TransformComponent;
		using result_type = std::size_t;
		result_type operator()(argument_type const& a) const
		{
			result_type const h1(std::hash<SyVector3>()(a._position));
			result_type const h2(std::hash<SyVector3>()(a.scale));
			result_type const h3(std::hash<SyVector3>()(a._rotation));
			result_type const h4(std::hash<uint32_t>()(a.parent));
			result_type const h5(std::hash<SyVector3>()(a.localPosition));
			result_type const h6(std::hash<SyVector3>()(a.localRotation));
			result_type const h7(std::hash<SyVector3>()(a.localScale));
			return h1 * 37 + (h1 * 37 + h2) * 37 +
				((h1 * 37 + h2) * 37 + h3) * 37+
				(((h1 * 37 + h2) * 37 + h3) * 37+h4)*37+
				((((h1 * 37 + h2) * 37 + h3) * 37 + h4) * 37 +h5)*37+
				(((((h1 * 37 + h2) * 37 + h3) * 37 + h4) * 37 + h5) * 37+h6)*37+
				((((((h1 * 37 + h2) * 37 + h3) * 37 + h4) * 37 + h5) * 37 + h6) * 37+h7);
		}
	};
}