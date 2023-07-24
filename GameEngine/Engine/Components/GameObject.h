#pragma once
#include "GameComponent.h"
#include "TransformComponent.h"
#include "MeshComponent.h"
#include "../DirectXSDK/entt/entt.hpp"
#include "../Core/Scene/Scene.h"
#include <set>
#include <vector>
#include <iostream>
#include <memory>


class EngineCore;

class GameObject
{
	friend class Scene;

public:
	entt::entity id=entt::entity(1);
	std::string name = "GameObject";

	GameObject();
	virtual void Initialize();
	
	entt::registry& GetRegistry();

	template <typename T>
	T& GetComponent()
	{
		return GetRegistry().get<T>(id);
	}

	template <typename T>
	void RemoveComponent()
	{
		GetRegistry().erase(id);
	}

	std::set<entt::entity>& GetChildrenObjects();
	
	std::tuple<bool, entt::entity> GetParentObjects();

	//ƒл€ тестировани€, потом удалить!
	void AddChild(GameObject* child);
	void RemoveChild(GameObject* child);
	void SetParent(GameObject* parent);
	
private:
	bool hasParent = false;
	entt::entity parentObject=(entt::entity)-1;
	std::set<entt::entity> childrenObjects;

	void RemoveChild(entt::entity childID)
	{
		childrenObjects.erase(childID);
	}

	void AddChild(entt::entity childID)
	{
		childrenObjects.insert(childID);
	}
};

//TODO: ѕеренести повтор€ющийс€ код в отдельный класс.


