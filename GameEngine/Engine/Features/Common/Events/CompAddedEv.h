#pragma once
#include "../ECompId.h"
#include "../../vendor/entt/entt.hpp"
struct CompAddedEv
{
    CompAddedEv(ECompId id, entt::entity ent) :
                            Id(id), Ent(ent) {}
    ECompId Id;
    entt::entity Ent;
};