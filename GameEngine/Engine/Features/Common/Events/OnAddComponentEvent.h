#pragma once
#include "../EComponentTypes.h"
#include "../../vendor/entt/entt.hpp"
struct SyOnAddComponentEvent
{
    SyOnAddComponentEvent(SyEComponentType componentType, entt::entity entity) :
                            ComponentType(componentType), Entity(entity) {}
    SyEComponentType ComponentType;
    entt::entity Entity;
};