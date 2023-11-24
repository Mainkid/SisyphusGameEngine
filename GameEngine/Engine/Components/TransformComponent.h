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

inline size_t hash_value(const TransformComponent& tf)
{
	size_t hash = 0;
	boost::hash_combine(hash, tf._position);
	boost::hash_combine(hash, tf._rotation);
	boost::hash_combine(hash, tf.scale);
	boost::hash_combine(hash, tf.parent);
	boost::hash_combine(hash, tf.localPosition);
	boost::hash_combine(hash, tf.localRotation);
	boost::hash_combine(hash, tf.localScale);
	return hash;
}