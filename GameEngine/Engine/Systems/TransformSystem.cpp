#include "TransformSystem.h"


void TransformSystem::Init()
{
	
}

void TransformSystem::Run()
{
	auto view = EngineCore::instance()->scene->registry.view<TransformComponent>();
	for (auto& entity :view)
	{
		TransformComponent& tc = view.get<TransformComponent>(entity);
		uint32_t hsh = hasher(tc);
		if (tc.hash != hsh)
		{
			TransformHelper::UpdateTransformMatrix(tc);
			//UpdateTransformMatrix(tc);
			tc.hash = hsh;
		}
	}
}

void TransformSystem::Destroy()
{

}



