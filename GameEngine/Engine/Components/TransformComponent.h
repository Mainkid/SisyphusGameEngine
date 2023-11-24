#pragma once
#include "SimpleMath.h"
#include "../../vendor/entt/entt.hpp"
#include "../Core/Tools/Structures/Vector.h"
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

	size_t MonoHash = 0; // read/write only by mono sync system


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
	template<> struct hash<Vector3>
	{
		size_t operator()(const Vector3& a) const
		{
			return HashCombine(a.x, a.y, a.z);
		}
	};
}

namespace std
{
	template<> struct hash<TransformComponent>
	{
		size_t operator()(const TransformComponent& a) const noexcept
		{
			return HashCombine(
				a._position,
				a._rotation,
				a.scale,
				a.parent,
				a.localPosition,
				a.localRotation,
				a.localScale
			);
		}
	};
}