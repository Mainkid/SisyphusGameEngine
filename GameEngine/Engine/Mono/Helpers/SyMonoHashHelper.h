#pragma once

struct TransformComponent;
struct MeshComponent;
struct SyRBodyComponent;

namespace mono
{
	class SyMonoHashHelper
	{
	public:
		static size_t Hash(const TransformComponent& comp);
		static size_t Hash(const MeshComponent& comp);
		static size_t Hash(const SyRBodyComponent& comp);
	};
}