#include "Scene.h"

Scene::Scene()
{

}

void Scene::Initialize()
{   
	

}

void Scene::Update(float deltaSec)
{
	/*for (auto& component : gameObjects)
		component->Update(deltaSec);*/
}

GameObject* Scene::AddGameObject()
{
	std::unique_ptr<GameObject> m_gameObject = std::make_unique<GameObject>();
	auto id = registry.create();
	registry.emplace<TransformComponent>(id);
	registry.emplace<MeshComponent>(id);
	m_gameObject->id = id;
	gameObjects[id] = std::move(m_gameObject);
	return gameObjects[id].get();
}

GameObject* Scene::AddLight(LightType _lightType)
{
	std::unique_ptr<GameObject> m_gameObject = std::make_unique<GameObject>();
	auto id = registry.create();
	registry.emplace<LightComponent>(id,_lightType);
	registry.emplace<TransformComponent>(id);
	m_gameObject->id = id;
	gameObjects[id] = std::move(m_gameObject);
	return gameObjects[id].get();
}

GameObject* Scene::AddParticleSystem()
{
	std::unique_ptr<GameObject> m_gameObject = std::make_unique<GameObject>();
	auto id = registry.create();
	registry.emplace<ParticleComponent>(id);
	registry.emplace<TransformComponent>(id);
	m_gameObject->id = id;
	gameObjects[id] = std::move(m_gameObject);
	return gameObjects[id].get();
}

bool Scene::DestroyGameObject(entt::entity _id)
{
	if (gameObjects[_id]->childrenObjects.size() > 0)
	{
		for (auto childID : gameObjects[_id]->childrenObjects)
			DestroyGameObject(childID);
	}

	if (gameObjects[_id]->hasParent)
	{
		RemoveChild(gameObjects[_id]->parentObject, _id);
	}
	
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

void Scene::SetParent(GameObject* sourceGameObject, GameObject* parentGameObject)
{
	if (sourceGameObject->hasParent)
		gameObjects[sourceGameObject->parentObject]->RemoveChild(sourceGameObject->id);

	if (parentGameObject)
	{
		sourceGameObject->hasParent = true;
		sourceGameObject->parentObject = parentGameObject->id;

		parentGameObject->AddChild(sourceGameObject->id);
	}
	else
	{
		sourceGameObject->hasParent = false;
	}
}

void Scene::SetParent(entt::entity sourceGameObject, entt::entity parentGameObject)
{
	if (parentGameObject == entt::null)
		SetParent(gameObjects[sourceGameObject].get(), nullptr);
	else if (!HasHierarchyCycles(sourceGameObject,parentGameObject))
		SetParent(gameObjects[sourceGameObject].get(), gameObjects[parentGameObject].get());
}



void Scene::AddChild(GameObject* parentGameObject, GameObject* childGameObject)
{
	SetParent(childGameObject, parentGameObject);
}

void Scene::AddChild(entt::entity parentGameObject, entt::entity childGameObject)
{
	SetParent(gameObjects[childGameObject].get(), gameObjects[parentGameObject].get());
}

void Scene::RemoveChild(GameObject* parentGameObject, GameObject* childGameObject)
{
	childGameObject->hasParent = false;
	parentGameObject->RemoveChild(childGameObject->id);
}

void Scene::RemoveChild(entt::entity parentGameObject, entt::entity childGameObject)
{
	RemoveChild(gameObjects[parentGameObject].get(), gameObjects[childGameObject].get());
}

bool Scene::HasHierarchyCycles(entt::entity sourceGameObject, entt::entity parentGameObject)
{
	if (parentGameObject == sourceGameObject)
		return true;

	bool res = false;
	for (auto& gameObject : gameObjects[sourceGameObject]->childrenObjects)
	{
		res = res || HasHierarchyCycles(gameObject, parentGameObject);
	}
	return res;
}
