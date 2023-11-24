#pragma once
#include "../../../vendor/entt/entt.hpp"
struct SyOnCreateColliderEvent
{
    SyOnCreateColliderEvent(entt::entity entity) : Entity(entity) {}
    entt::entity Entity; //entity that collider is attached to
};