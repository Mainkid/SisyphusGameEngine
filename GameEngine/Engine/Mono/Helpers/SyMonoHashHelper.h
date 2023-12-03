#pragma once

struct GameObjectComp;
struct SyPrimitiveColliderComponent;
struct LightComponent;
struct TransformComponent;
struct MeshComponent;
struct SyRBodyComponent;

namespace mono
{
	class SyMonoHashHelper
	{
	public:
		static size_t Hash(const GameObjectComp& go);
		static size_t Hash(const TransformComponent& tf);
		static size_t Hash(const MeshComponent& mesh);
		static size_t Hash(const LightComponent& light);
		static size_t Hash(const SyPrimitiveColliderComponent& collider);
		static size_t Hash(const SyRBodyComponent& rigid);
	};
}