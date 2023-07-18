#include "../Core/EngineCore.h"
#include "GameObject.h"


GameObject::GameObject(EngineCore* _engine)
{
	this->engine = _engine;
}

void GameObject::Initialize()
{
	

}

entt::registry& GameObject::GetRegistry()
{
	return engine->scene->registry;
}




