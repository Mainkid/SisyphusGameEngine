#include "TransformSystem.h"
#include "EngineContext.h"
#include "../Core/ServiceLocator.h"
#include "TransformHelper.h"

void TransformSystem::Init()
{
	ec = ServiceLocator::instance()->Get<EngineContext>();
}

void TransformSystem::Run()
{
	auto view = _ecs->view<TransformComponent>();
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



