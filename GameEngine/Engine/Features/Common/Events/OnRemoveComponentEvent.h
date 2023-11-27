#pragma once
#include "../EComponentTypes.h"
#include "../../vendor/entt/entt.hpp"

struct SyOnRemoveComponentEvent
{
    SyOnRemoveComponentEvent(SyEComponentTypes componentType, entt::entity entity) :
                            ComponentType(componentType), Entity(entity) {}
    SyEComponentTypes ComponentType;
    entt::entity Entity;
};