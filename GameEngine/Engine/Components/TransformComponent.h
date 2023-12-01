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
	TransformComponent(const TransformComponent& tc)
	{
		//this->_position = tc._position;
		//this->_rotation = tc._rotation;
		//this->scale = tc.scale;
		this->localPosition = tc.localPosition;
		this->localRotation = tc.localRotation;
		this->localScale = tc.localScale;
		this->worldHash = 0;
		this->localHash = 0;
		this->parent = tc.parent;
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