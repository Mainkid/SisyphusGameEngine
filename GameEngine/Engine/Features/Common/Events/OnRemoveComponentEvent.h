#pragma once
#include "../EComponentTypes.h"
#include "../../vendor/entt/entt.hpp"

struct SyOnRemoveComponentEvent
{
    SyOnRemoveComponentEvent(ESyComponentType componentType, entt::entity entity) :
                            ComponentType(componentType), Entity(entity) {}
    ESyComponentType ComponentType;
    entt::entity Entity;
};