#include "TransformSystem.h"
#include "EngineContext.h"
#include "../Core/ServiceLocator.h"
#include "TransformHelper.h"

SyResult TransformSystem::Init()
{
	SyResult result;
	ec = ServiceLocator::instance()->Get<EngineContext>();
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "Transform system initialization successful. ");
	return result;
}

SyResult TransformSystem::Run()
{
	SyResult result;
	auto view = ec->scene->registry.view<TransformComponent>();
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
	return result;
}

SyResult TransformSystem::Destroy()
{
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "Transform system destruction successful. ");
	return SyResult();
}



