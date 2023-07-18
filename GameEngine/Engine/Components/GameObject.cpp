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




