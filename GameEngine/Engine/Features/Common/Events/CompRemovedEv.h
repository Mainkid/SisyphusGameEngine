#pragma once
#include "../ECompId.h"
#include "../../vendor/entt/entt.hpp"

struct CompRemovedEv
{
    CompRemovedEv(ECompId id, entt::entity ent) :
                            Id(id), Ent(ent) {}
    ECompId Id;
    entt::entity Ent;
};