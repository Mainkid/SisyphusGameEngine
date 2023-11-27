#pragma once
#include "../EComponentTypes.h"
#include "../../vendor/entt/entt.hpp"
struct SyOnAddComponentEvent
{
    SyOnAddComponentEvent(SyEComponentTypes componentType, entt::entity entity) :
                            ComponentType(componentType), Entity(entity) {}
    SyEComponentTypes ComponentType;
    entt::entity Entity;
};