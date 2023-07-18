#pragma once
#include "../../DirectXSDK/entt/entt.hpp"
#include <vector>
#include <memory>

#include "../../Components/GameObject.h"
#include "../../Components/LightComponent.h"


class Scene
{

public:
	std::map<entt::entity,std::unique_ptr<GameObject>> gameObjects;
	entt::registry registry;

	Scene();
	~Scene() = default;
	void Initialize();
	void Update(float deltaSec);
	void Render() {};

	GameObject* AddGameObject();
	GameObject* AddLight(LightType _lightType);
	bool DestroyGameObject(entt::entity);
	bool DestroyGameObject(GameObject* _gameObject);

private:
	
};

