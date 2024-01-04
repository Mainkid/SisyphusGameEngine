#pragma once
#include "../ECompId.h"
#include "../../vendor/entt/entt.hpp"

struct CompRemovedEv
{
    CompRemovedEv(ECompId id, entt::entity ent) :
                            Id(id), Ent(ent), IsFromMono(false) {}

    CompRemovedEv(ECompId id, entt::entity ent, bool isFromMono) :
        Id(id), Ent(ent), IsFromMono(isFromMono) {}

    ECompId Id;
    entt::entity Ent;
    bool IsFromMono;
};