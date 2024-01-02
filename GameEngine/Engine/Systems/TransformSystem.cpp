#include "TransformSystem.h"
#include "../Contexts/EngineContext.h"
#include "../Core/ServiceLocator.h"
#include "TransformHelper.h"
#include "../Events/SyHotReloadEvent.h"
#include "../Scene/GameObjectHelper.h"
#include "../Events/SySceneLoadEvent.h"

SyResult TransformSystem::Init()
{
	SyResult result;
	ec = ServiceLocator::instance()->Get<EngineContext>();
	return result;
}

SyResult TransformSystem::Run()
{
	SyResult result;
	auto view = _ecs->view<TransformComponent>();

	auto eventView = SY_GET_THIS_FRAME_EVENT_VIEW(SySceneLoadEvent);

	auto view2 = SY_GET_THIS_FRAME_EVENT_VIEW(SyHotReloadEvent);

	// if (view2.size_hint()>0)
	// {
	// 	std::cout << std::endl;
	// }

	//NOT WORKING!!!
	// 
	//for (auto& entity : eventView)
	//{
	//	SySceneLoadEvent& testEvent = eventView.get<SySceneLoadEvent>(entity);	//ïîëó÷èëè ñàìî ñîáûòèå (îáúåêò) ñî âñåìè ïåðåäàííûìè â íåãî äàííûìè
	//	SY_LOG_EVSY(SY_LOGLEVEL_WARNING, testEvent.message.c_str());	//ëîãèêà îáðàáîòêè èâåíòà
	//}


	if (eventView.size_hint() > 0)
	{
		ser::Serializer& ser = ServiceLocator::instance()->Get<EngineContext>()->serializer;
		for (auto& entity : view)
		{
			TransformComponent& tc = view.get<TransformComponent>(entity);
			tc.parent = static_cast<uint32_t>(ser.GetContextEntityToEntity(static_cast<entt::entity>(tc.parent)));

		}

		for (auto& entity : view)
		{
			TransformComponent& tc = view.get<TransformComponent>(entity);
			GameObjectHelper::AddChild(_ecs, static_cast<entt::entity>(tc.parent), entity);
		}
	}

	for (auto& entity : view)
	{
		TransformComponent& tc = view.get<TransformComponent>(entity);

		size_t wHash = 0;
		boost::hash_combine(wHash, tc._position);
		boost::hash_combine(wHash, tc._rotation);
		boost::hash_combine(wHash, tc.scale);

		size_t lHash = 0;
		boost::hash_combine(lHash, tc.localPosition);
		boost::hash_combine(lHash, tc.localRotation);
		boost::hash_combine(lHash, tc.localScale);
		
		if (tc.worldHash != wHash)
		{
			tc.worldHash = wHash;
			Matrix parentTransform = Matrix::Identity;
			if (tc.parent != entt::null)
			{
				TransformComponent& parentTc = _ecs->get<TransformComponent>((entt::entity)tc.parent);
				parentTransform = parentTc.transformMatrix;
			}
			TransformHelper::UpdateWorldTransformMatrix(tc, parentTransform);
		}
		if (tc.localHash != lHash)
		{
			tc.localHash = lHash;
			TransformHelper::UpdateTransformMatrix(tc);
		}

	}
	return result;
}

SyResult TransformSystem::Destroy()
{
	return SyResult();
}



