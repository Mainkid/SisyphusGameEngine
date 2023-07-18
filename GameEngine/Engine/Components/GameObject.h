#pragma once
#include "GameComponent.h"
#include "TransformComponent.h"
#include "MeshComponent.h"
#include "../DirectXSDK/entt/entt.hpp"
#include <vector>
#include <iostream>
#include <memory>


class EngineCore;

class GameObject
{


public:
	entt::entity id=entt::entity(1);

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
	
	
};

//TODO: ѕеренести повтор€ющийс€ код в отдельный класс.


