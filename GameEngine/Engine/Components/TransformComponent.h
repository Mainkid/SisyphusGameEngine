#pragma once
#include "SimpleMath.h"
#include "../../vendor/entt/entt.hpp"


using namespace DirectX::SimpleMath;
struct TransformComponent
{
	TransformComponent() = default;
	TransformComponent(Vector3 _pos, Vector3 _rot, Vector3 _scale)
	{
		this->position = _pos;
		this->rotation = _rot;
		this->scale = _scale;
	}
	Vector3 position = Vector3(0, 0, 0);
	Vector3 rotation = Vector3(0, 0, 0);
	Vector3 scale = Vector3(1, 1, 1);
	Matrix translationMatrix=Matrix::Identity;
	Matrix rotationMatrix=Matrix::Identity;
	Matrix scaleMatrix=Matrix::Identity;
	uint32_t hash = 0;
	entt::entity parent = entt::null;
	std::set<entt::entity> children = {};
};

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
			result_type const h1(std::hash<Vector3>()(a.position));
			result_type const h2(std::hash<Vector3>()(a.scale));
			result_type const h3(std::hash<Vector3>()(a.rotation));
			return h1 * 37 + (h1 * 37 + h2) * 37 +
				((h1 * 37 + h2) * 37 + h3) * 37;
		}
	};
}