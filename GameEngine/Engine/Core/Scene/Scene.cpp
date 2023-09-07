#include "Scene.h"

Scene::Scene()
{

}

void Scene::Initialize()
{   
	

}

void Scene::Update(float deltaSec)
{

}

entt::entity Scene::AddGameObject()
{
	auto id = registry.create();
	registry.emplace<TransformComponent>(id);
	registry.emplace<MeshComponent>(id);
	gameObjects.insert(id);
	return id;
}

entt::entity Scene::AddLight(LightType _lightType)
{
	auto id = registry.create();
	registry.emplace<LightComponent>(id,_lightType);
	registry.emplace<TransformComponent>(id);
	gameObjects.insert(id);
	return id;
}

entt::entity Scene::AddParticleSystem()
{
	auto id = registry.create();
	registry.emplace<ParticleComponent>(id);
	registry.emplace<TransformComponent>(id);
	gameObjects.insert(id);
	return id;
}

bool Scene::DestroyGameObject(entt::entity _id)
{
	return true;
	if (registry.get<TransformComponent>(_id).children.size() > 0)
	{
		for (auto childID : registry.get<TransformComponent>(_id).children)
			DestroyGameObject(childID);
	}

	if (registry.get<TransformComponent>(_id).parent!=entt::null)
	{
		RemoveChild(registry.get<TransformComponent>(_id).parent, _id);
	}
	
	if (gameObjects.count(_id))
	{
		registry.destroy(_id);
		gameObjects.erase(_id);
		return true;
	}
	return false;
}


void Scene::SetParent(entt::entity sourceGameObject, entt::entity parentGameObject)
{
	if (parentGameObject == entt::null)
	{
		if (registry.get<TransformComponent>(sourceGameObject).parent != entt::null)
			RemoveChild(registry.get<TransformComponent>(sourceGameObject).parent, sourceGameObject);
		registry.get<TransformComponent>(sourceGameObject).parent = entt::null;
	}
	else if (!HasHierarchyCycles(sourceGameObject, parentGameObject))
	{
		registry.get<TransformComponent>(sourceGameObject).parent = parentGameObject;
		registry.get<TransformComponent>(parentGameObject).children.insert(sourceGameObject);
	}
}   

void Scene::AddChild(entt::entity parentGameObject, entt::entity childGameObject)
{
	SetParent(childGameObject, parentGameObject);
}

void Scene::RemoveChild(entt::entity parentGameObject, entt::entity childGameObject)
{
	registry.get<TransformComponent>(parentGameObject).children.erase(childGameObject);
	//RemoveChild(parentGameObject,childGameObject);
}

bool Scene::HasHierarchyCycles(entt::entity sourceGameObject, entt::entity parentGameObject)
{
	if (parentGameObject == sourceGameObject)
		return true;

	bool res = false;
	for (auto& gameObject : registry.get<TransformComponent>(sourceGameObject).children)
	{
		res = res || HasHierarchyCycles(gameObject, parentGameObject);
	}
	return res;
}
