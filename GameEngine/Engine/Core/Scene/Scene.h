#pragma once
#include "../../DirectXSDK/entt/entt.hpp"
#include <vector>
#include <memory>

#include "../../Components/GameObject.h"
#include "../../Components/LightComponent.h"


class Scene
{
	friend class GameObject;
	friend class HierarchyWidget;
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
	void SetParent(GameObject* sourceGameObject, GameObject* parentGameObject);
	void SetParent(entt::entity sourceGameObject, entt::entity parentGameObject);
	void AddChild(GameObject* parentGameObject, GameObject* childGameObject);
	void AddChild(entt::entity parentGameObject, entt::entity childGameObject);  
	void RemoveChild(GameObject* parentGameObject, GameObject* childGameObject);
	void RemoveChild(entt::entity parentGameObject, entt::entity childGameObject);
	bool HasHierarchyCycles(entt::entity childGameObject, entt::entity parentGameObject);
	
};

