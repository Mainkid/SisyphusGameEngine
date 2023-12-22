#pragma once
#include "../EComponentTypes.h"
#include "../../vendor/entt/entt.hpp"
struct SyOnAddComponentEvent
{
    SyOnAddComponentEvent(ESyComponentType componentType, entt::entity entity) :
                            ComponentType(componentType), Entity(entity) {}
    ESyComponentType ComponentType;
    entt::entity Entity;
};