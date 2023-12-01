#include "TransformCheckUpSystem.h"
#include "../../Components/TransformComponent.h"
#include <boost/functional/hash.hpp>

SyResult SyTransformCheckUpsystem::Init()
{
    return SyResult();
}

SyResult SyTransformCheckUpsystem::Run()
{
    auto view = _ecs->view<TransformComponent>();
    for (auto& entity : view)
    {
        auto& transformC = view.get<TransformComponent>(entity);
        size_t wHash = 0;
        boost::hash_combine(wHash, transformC._position);
        boost::hash_combine(wHash, transformC._rotation);
        boost::hash_combine(wHash, transformC.scale);

        size_t lHash = 0;
        boost::hash_combine(lHash, transformC.localPosition);
        boost::hash_combine(lHash, transformC.localRotation);
        boost::hash_combine(lHash, transformC.localScale);

        if (transformC.worldHash != wHash|| transformC.localHash != lHash)
            transformC._wasChangedFromOutside = true;
        else
            transformC._wasChangedFromOutside = false;
    }
    return SyResult();
}

SyResult SyTransformCheckUpsystem::Destroy()
{
    return SyResult();
}
