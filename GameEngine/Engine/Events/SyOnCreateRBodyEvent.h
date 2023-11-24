#pragma once
#include "../../../vendor/entt/entt.hpp"
struct SyOnCreateRBodyEvent
{
    SyOnCreateRBodyEvent(entt::entity& entity) : Entity(entity) {}
    entt::entity Entity; //entity to which RigidBody Component is attached to
};