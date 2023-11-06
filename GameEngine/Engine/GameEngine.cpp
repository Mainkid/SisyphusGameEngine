#include <iostream>
#include <uuid.hpp>
#include "Components/EditorBillboardComponent.h"
#include "Components/GameObjectComp.h"
#include "Components/MeshComponent.h"
#include "Components/TransformComponent.h"
#include "Core/EngineCore.h"
#include "Scene/GameObjectHelper.h"

#include "SimpleMath.h"
#include "Serialization/Serializable.h"

#define SY_PI 3.14f
#define SY_PI2 SY_PI / 2


int main()
{
    EngineCore::instance()->StartUp();
    
    entt::registry* ecs = &ServiceLocator::instance()->Get<EngineContext>()->ecs;
    EngineContext* ec = EngineCore::instance()->ec;
    auto lightDir = GameObjectHelper::CreateLight(ecs, ELightType::Directional);

    auto light = GameObjectHelper::CreateLight(ecs, ELightType::Ambient);
    ecs->get<LightComponent>(light).Color = Vector4(0.15f, 0.15f, 0.15f, 0.15f);

    auto lightPoint = GameObjectHelper::CreateLight(ecs, ELightType::PointLight);
    ecs->get<LightComponent>(lightPoint).ParamsRadiusAndAttenuation = Vector4(3.0f, 0.0f, 0.0f, 1.0f);
    ecs->get<LightComponent>(lightPoint).Color = Vector4(1, 1, 0.0f, 3.0f);
    ecs->get<TransformComponent>(lightPoint).position = Vector3(3, 0, 0);

    auto lightPoint2 = GameObjectHelper::CreateLight(ecs, ELightType::PointLight);

    ecs->get<LightComponent>(lightPoint2).ParamsRadiusAndAttenuation = Vector4(3.0f, 0.0f, 0.0f, 1.0f);
    ecs->get<LightComponent>(lightPoint2).Color = Vector4(1, 0, 0.0f, 3.0f);
    ecs->get<TransformComponent>(lightPoint2).position = Vector3(3, 0, 0);
    ecs->get<LightComponent>(lightPoint2).LightBehavior = LightBehavior::Movable;

    auto obj = GameObjectHelper::Create(ecs, "RigidBody", { 0.0f, 0.0f, 0.0f });
    auto result = GameObjectHelper::AddRigidBodyComponent(ecs, obj, SY_RB_TYPE_DYNAMIC);
    
    //---------- Serialization test ----------------
    // ser::Serializer& ser = ServiceLocator::instance()->Get<EngineContext>()->serializer;
    // ser.AddEcsCompMeta<GameObjectComp>();
    // ser.AddEcsCompMeta<TransformComponent>();
    // ser.AddEcsCompMeta<MeshComponent>();
    // ser.AddEcsCompMeta<LightComponent>();
    // ser.AddEcsCompMeta<EditorBillboardComponent>();
    //
    // auto json = ser.Serialize<GameObjectComp>(*ecs);
    // std::ofstream file;
    // file.open("scene.json", std::ios::trunc);
    // file << std::setw(1) << json;
    // file.close();
    //
    // auto view = ecs->view<GameObjectComp>();
    // for (auto ent : view)
    //     ecs->destroy(ent);
    //
    // ser.Deserialize(json, *ecs);
    //---------------------------------------------


    EngineCore::instance()->Update();
    EngineCore::instance()->ShutDown();
    

    
}
