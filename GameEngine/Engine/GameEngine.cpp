#include <iostream>
#include <uuid.hpp>

#include "Core/EngineCore.h"
#include "Components/EditorBillboardComponent.h"
#include "Components/GameObjectComp.h"
#include "Features/Mesh/Components/MeshComponent.h"
#include "Components/TransformComponent.h"

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

   /* auto lightPoint = GameObjectHelper::CreateLight(ecs, ELightType::PointLight);
    ecs->get<LightComponent>(lightPoint).ParamsRadiusAndAttenuation = Vector4(3.0f, 0.0f, 0.0f, 1.0f);
    ecs->get<LightComponent>(lightPoint).Color = Vector4(1, 1, 0.0f, 3.0f);
    ecs->get<TransformComponent>(lightPoint)._position = Vector3(3, 0, 0);

    auto lightPoint2 = GameObjectHelper::CreateLight(ecs, ELightType::PointLight);*/

    //ecs->get<LightComponent>(lightPoint2).ParamsRadiusAndAttenuation = Vector4(3.0f, 0.0f, 0.0f, 1.0f);
    //ecs->get<LightComponent>(lightPoint2).Color = Vector4(1, 0, 0.0f, 3.0f);
    //ecs->get<TransformComponent>(lightPoint2)._position = Vector3(3, 0, 0);
    //ecs->get<LightComponent>(lightPoint2).LightBehavior = LightBehavior::Movable;

    auto base = GameObjectHelper::Create(ecs, "Base", { 0.0f, -5.0f, 13.0f });
    ecs->get<TransformComponent>(base).scale = {10.0f, 1.0f, 10.0f};
    auto result1 = GameObjectHelper::AddRigidBodyComponent(ecs, base, STATIC);
    auto result2 = GameObjectHelper::AddCubeMeshComponent(ecs, base);
    SyPrimitiveColliderShapeDesc baseColDesc;
    baseColDesc.Extent = { 10.0f, 1.0f, 10.0f };
    auto result3 = GameObjectHelper::AddPrimitiveColliderComponent(ecs, base,
                                                            BOX,
                                                            baseColDesc,
                                                            SyColliderMaterial());

    
    auto meshUuid = ServiceLocator::instance()->Get<ResourceService>()->GetUUIDFromPath(".\\Game\\Assets\\fbx\\barrel.fbx");
    auto staticMesh = GameObjectHelper::Create(ecs, "Static Mesh", {0.0f, -2.5f, 8.0f});
    ecs->get<TransformComponent>(staticMesh).scale = { 3.0f, 3.0f, 3.0f };
    auto result4 = GameObjectHelper::AddMeshComponent(ecs, staticMesh, meshUuid, SyEMeshComponentFlags::MESH_COLLIDER | SyEMeshComponentFlags::MESH_RENDER);
    auto result5 = GameObjectHelper::AddRigidBodyComponent(ecs, staticMesh, DYNAMIC, 1, SyERBodyFlags::KINEMATIC | SyERBodyFlags::USE_DENSITY);
    auto result6 = GameObjectHelper::AddTrimeshColliderComponent(ecs, staticMesh, SyColliderMaterial());

    auto cube1 = GameObjectHelper::Create(ecs, "Cube1", { 0.0f, 5.0f, 8.0f });
    auto result7 = GameObjectHelper::AddRigidBodyComponent(ecs, cube1, DYNAMIC);
    auto result8 = GameObjectHelper::AddCubeMeshComponent(ecs, cube1);
    SyPrimitiveColliderShapeDesc cubeColDesc;
    cubeColDesc.Extent = { 1.0f, 1.0f, 1.0f };
    auto result9 = GameObjectHelper::AddPrimitiveColliderComponent(ecs, cube1,
        BOX,
        cubeColDesc,
        SyColliderMaterial());

    ecs->get<SyRBodyComponent>(cube1).LinearVelocity = SyVector3(5.0f, 0.0f, 0.0f);
    

    
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
