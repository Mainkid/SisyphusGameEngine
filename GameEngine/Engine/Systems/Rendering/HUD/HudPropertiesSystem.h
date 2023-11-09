﻿#pragma once
#include "../../Core/ECS/SystemBase.h"
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <boost/lexical_cast.hpp>
#include "../../../../vendor/entt/entt.hpp"
#include <utility>

namespace mono
{
	class SyMonoEditor;
}

struct EngineContext;
struct HardwareContext;
struct RenderContext;
class ResourceService;


class HudPropertiesSystem : public SystemBase
{
public:
    SyResult Init() override;
    SyResult Run() override;
    SyResult Destroy() override;
private:
    EngineContext* ec;
    RenderContext* rc;
    HardwareContext* hc;
    ResourceService* rs;
    mono::SyMonoEditor* _monoEditor;

    std::string windowID;
    boost::uuids::uuid prevSelectedUUID = boost::uuids::nil_uuid();
    bool isFocused;
    
    void DrawMaterialProperties();
    void UpdateHudProperties(bool);

};
