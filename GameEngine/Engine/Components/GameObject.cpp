#include "../Core/EngineCore.h"
#include "GameObject.h"


GameObject::GameObject()
{

}

void GameObject::Initialize()
{
	

}

entt::registry& GameObject::GetRegistry()
{
	return EngineCore::instance()->scene->registry;
}

std::set<entt::entity>& GameObject::GetChildrenObjects()
{
	return childrenObjects;
}


std::tuple<bool,entt::entity> GameObject::GetParentObjects()
{
	return std::make_tuple(hasParent, parentObject);
}

void GameObject::AddChild(GameObject* child)
{
	EngineCore::instance()->scene->AddChild(this, child);
}

void GameObject::RemoveChild(GameObject* child)
{
	EngineCore::instance()->scene->RemoveChild(this, child);
}

void GameObject::SetParent(GameObject* parent)
{
	EngineCore::instance()->scene->SetParent(this, parent);
}



