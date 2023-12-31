﻿    #include <iostream>
#include <uuid.hpp>

#include "Core/EngineCore.h"
#include "Components/EditorBillboardComponent.h"
#include "Components/GameObjectComp.h"
#include "Features/Mesh/Components/MeshComponent.h"
#include "Components/TransformComponent.h"

#include "Scene/GameObjectHelper.h"

#include "SimpleMath.h"
#include "Features/Physics/Components/JointComponent.h"
#include "Serialization/Serializable.h"

#include "Features/AI/Components/NavMeshComponent.h"


#define SY_PI 3.14f
#define SY_PI2 SY_PI / 2


int main()
{
    EngineCore::instance()->StartUp();
    EngineCore::instance()->Update();
    EngineCore::instance()->ShutDown();

    return 0;
    
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


#pragma region Test Base
     //auto base = GameObjectHelper::Create(ecs, "Base", { 0.0f, -5.0f, 13.0f });
     //ecs->get<TransformComponent>(base).scale = {10.0f, 1.0f, 10.0f};
     //auto result1 = GameObjectHelper::AddRigidBodyComponent(ecs, base, STATIC);
     //auto result2 = GameObjectHelper::AddCubeMeshComponent(ecs, base);
     //ecs->get<MeshComponent>(base).flags |= AFFECTS_NAVMESH;
     //SyPrimitiveColliderShapeDesc baseColDesc;
     //baseColDesc.Extent = { 10.0f, 1.0f, 10.0f };
     //auto result3 = GameObjectHelper::AddPrimitiveColliderComponent(ecs, base,
     //                                                        BOX,
     //                                                        baseColDesc,
     //                                                        SyColliderMaterial());
#pragma endregion 

#pragma region Test Mesh
     auto meshUuid = ServiceLocator::instance()->Get<ResourceService>()->GetUUIDFromPath(".\\Game\\Assets\\dungeon.fbx");
     auto staticMesh = GameObjectHelper::Create(ecs, "Static Mesh", SyVector3::ZERO);
     //ecs->get<TransformComponent>(staticMesh).scale = { 3.0f, 3.0f, 3.0f };
     auto result4 = GameObjectHelper::AddMeshComponent(ecs, staticMesh, meshUuid, SyEMeshComponentFlags::MESH_RENDER | SyEMeshComponentFlags::AFFECTS_NAVMESH);
     //auto result5 = GameObjectHelper::AddRigidBodyComponent(ecs, staticMesh, DYNAMIC, 1, SyERBodyFlags::KINEMATIC | SyERBodyFlags::USE_DENSITY);
     //auto result6 = GameObjectHelper::AddTrimeshColliderComponent(ecs, staticMesh, SyColliderMaterial());
#pragma endregion
#pragma region Test Cube 1
    // auto cube1 = GameObjectHelper::Create(ecs, "Cube1", { 0.0f, 5.0f, 8.0f });
    // auto result7 = GameObjectHelper::AddRigidBodyComponent(ecs, cube1, DYNAMIC, 10/*, SyERBodyFlags::DISABLE_GRAVITY*/);
    // auto result8 = GameObjectHelper::AddCubeMeshComponent(ecs, cube1);
    // SyPrimitiveColliderShapeDesc cubeColDesc;
    // cubeColDesc.Extent = { 1.0f, 1.0f, 1.0f };
    // auto result9 = GameObjectHelper::AddPrimitiveColliderComponent(ecs, cube1,
    //     BOX,
    //     cubeColDesc,
    //     SyColliderMaterial());
    //
    // auto cube2 = GameObjectHelper::Create(ecs, "Cube1", { 1.0f, 0.0f, 8.0f });
    // auto result13 = GameObjectHelper::AddRigidBodyComponent(ecs, cube2, DYNAMIC, 10/*, SyERBodyFlags::DISABLE_GRAVITY*/);
    // auto result14 = GameObjectHelper::AddCubeMeshComponent(ecs, cube2);
    // auto result15 = GameObjectHelper::AddPrimitiveColliderComponent(ecs, cube2,
    //     BOX,
    //     cubeColDesc,
    //     SyColliderMaterial());
    
#pragma endregion
#pragma region Test Sphere 1
    // auto sphere1 = GameObjectHelper::Create(ecs, "Sphere", { 1.0f, 0.0f, 8.0f });
    // auto result10 = GameObjectHelper::AddRigidBodyComponent(ecs, sphere1, DYNAMIC, 2/*, SyERBodyFlags::DISABLE_GRAVITY*/);
    // auto result11 = GameObjectHelper::AddSphereMeshComponent(ecs, sphere1);
    // SyPrimitiveColliderShapeDesc sphereColDesc; 
    // sphereColDesc.Radius = 1;
    // auto result12 = GameObjectHelper::AddPrimitiveColliderComponent(ecs, sphere1,
    //     SPHERE,
    //     sphereColDesc,
    //     SyColliderMaterial());
    // ecs->emplace<SyHingeJointComponent>(cube1, SyVector3(1.0f, 1.0f, 0.0f),
    //     SyVector3(0.0f, SyMathHelper::PI / 2, 0.0f));
    //ecs->emplace<SyFixedJointComponent>(cube2, cube1);

#pragma endregion
    
    auto running = GameObjectHelper::Create(ecs, "Running", Vector3(0, 0, 0));
    GameObjectHelper::AddMeshComponent(ecs, running, ServiceLocator::instance()->Get<ResourceService>()->GetUUIDFromPath("Game\\Assets\\Anims\\Runner.dae"));
    GameObjectHelper::AddAnimatorComponent(ecs, running);
    
    GameObjectHelper::CreateCamera(ecs);
    
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
   
   
#pragma region NavMesh
    auto navMeshEntity = GameObjectHelper::Create(ecs, "NavMesh", { 0.0f, -5.0f, 13.0f });
    auto result01 = ecs->emplace<SyNavMeshComponent>(navMeshEntity);
    auto& nmc = ecs->get<SyNavMeshComponent>(navMeshEntity);
    nmc.origin = { 0.0f, -5.0f, 13.0f };
    nmc.extent = SyVector3::ONE * 100.0f;
#pragma endregion

    //sound
    GameObjectHelper::CreateSoundComponent(ecs);
    GameObjectHelper::CreateSoundComponent(ecs);

    EngineCore::instance()->Update();
    EngineCore::instance()->ShutDown();   
}
