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
	ec->transformSystemPtr = this;
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

	static bool mustUpdateChildrenIncremental = true;
	for (auto& entity :view)
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

		tc._wasChangedFromEditor = false;
		if (tc.worldHash != wHash)
		{
			tc._wasChangedFromEditor = true;
			tc.worldHash = wHash;
			Matrix parentTransform = Matrix::Identity;
			if (tc.parent != entt::null)
			{
				TransformComponent& parentTc = _ecs->get<TransformComponent>((entt::entity)tc.parent);
				parentTransform = parentTc.transformMatrix;
			}
			TransformHelper::UpdateWorldTransformMatrix(tc,parentTransform);
		}
		if (tc.localHash != lHash )
		{
			tc._wasChangedFromEditor = true;
			tc.localHash = lHash;
			TransformHelper::UpdateTransformMatrix(tc, mustUpdateChildrenIncremental);
		}

	}
	return result;
}

SyResult TransformSystem::Destroy()
{
	return SyResult();
}



