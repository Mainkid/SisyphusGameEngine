#include "Scene.h"

Scene::Scene(EngineCore* engine)
{
	this->engine = engine;
}

void Scene::Initialize()
{   
	
	auto view= registry.view<TransformComponent>();
	for (auto entity : view)
	{
		TransformComponent& transform = registry.get<TransformComponent>(entity);
		transform.Initialize();
	}
}

void Scene::Update(float deltaSec)
{
	/*for (auto& component : gameObjects)
		component->Update(deltaSec);*/
}

GameObject* Scene::AddGameObject()
{
	std::unique_ptr<GameObject> m_gameObject = std::make_unique<GameObject>(engine);
	auto id = registry.create();
	registry.emplace<TransformComponent>(id);
	registry.emplace<MeshComponent>(id,engine);
	m_gameObject->id = id;
	gameObjects[id] = std::move(m_gameObject);
	return gameObjects[id].get();
}

GameObject* Scene::AddLight(LightType _lightType)
{
	std::unique_ptr<GameObject> m_gameObject = std::make_unique<GameObject>(engine);
	auto id = registry.create();
	registry.emplace<LightComponent>(id,engine,_lightType);
	registry.emplace<TransformComponent>(id);
	m_gameObject->id = id;
	gameObjects[id] = std::move(m_gameObject);
	return gameObjects[id].get();
}

bool Scene::DestroyGameObject(entt::entity _id)
{
	
	if (gameObjects.count(_id))
	{
		registry.destroy(_id);
		gameObjects.erase(_id);
		return true;
	}
	return false;
}

bool Scene::DestroyGameObject(GameObject* _gameObject)
{
	return DestroyGameObject(_gameObject->id);
}
