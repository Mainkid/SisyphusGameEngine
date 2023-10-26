#include "TransformSystem.h"
#include "EngineContext.h"
#include "../Core/ServiceLocator.h"
#include "TransformHelper.h"
#include "../Scene/GameObjectHelper.h"
#include "../Core/ECS/Events/SySceneLoadEvent.h"

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
	auto view = _ecs->view<TransformComponent>();

	auto eventView = SY_GET_THIS_FRAME_EVENT_VIEW(SySceneLoadEvent); 		

	//NOT WORKING

	//for (auto& entity : eventView)
	//{
	//	SySceneLoadEvent& testEvent = eventView.get<SySceneLoadEvent>(entity);	//�������� ���� ������� (������) �� ����� ����������� � ���� �������
	//	SY_LOG_EVSY(SY_LOGLEVEL_WARNING, testEvent.message.c_str());	//������ ��������� ������
	//}


	if (eventView.size_hint()>0)
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

	for (auto& entity :view)
	{
		TransformComponent& tc = view.get<TransformComponent>(entity);
		uint32_t hsh = hasher(tc);
		if (tc.hash != hsh)
		{
 			TransformHelper::UpdateTransformMatrix(tc);
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



