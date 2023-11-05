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
    //std::unique_ptr<Material> material = std::make_unique<Material>();
    //material->albedoTexturePath = "./Engine/Assets/PBR_test/sspere_diff.png";
    //material->metallicTexturePath = "./Engine/Assets/PBR_test/SS_LP_metallic.png";
    //material->normalmapTexturePath = "./Engine/Assets/PBR_test/SS_LP_normal.png";
    ////material->emissiveTexturePath = "./Engine/Assets/PBR_test/Crystal_stone_emissive.jpg";
    //material->roughnessTexturePath = "./Engine/Assets/PBR_test/SS_LP_roughness.png";
    //material->roughnessValue = (1, 0, 0, 0);

    EngineCore::instance()->StartUp();

    

    entt::registry* ecs = &ServiceLocator::instance()->Get<EngineContext>()->ecs;
    
    //EngineCore* engine = new EngineCore();
    
    EngineContext* ec = EngineCore::instance()->ec;
  /*  auto box1 = _ec->scene->AddStaticBox({ -5.0f, -5.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 3.0f, 1.0f, 3.0f });
    auto box2 = _ec->scene->AddDynamicBox({ -5.0f, 10.0f, 0.0f }, { SY_PI2 / 2, 0.0f, SY_PI2 / 2 });*/
    //auto _go = EngineCore::instance()->_ec->scene->AddGameObject();
    //auto _go1 = EngineCore::instance()->_ec->scene->AddGameObject();
    //auto _go2 = EngineCore::instance()->_ec->scene->AddGameObject();
    ////_go->GetComponent<TransformComponent>().SetPosition(Vector3(-3, 0, 0));
    //auto t = _go->GetComponent<TransformComponent>();
    //
    //GameObject* _go2 = EngineCore::instance()->scene->AddGameObject();
    //_go2->SetParent(_go);
    //_ec->scene->registry.get<MeshComponent>(_go).modelPath = "./Engine/Assets/sphere.fbx";
    //_ec->scene->registry.get<MeshComponent>(_go).texturePath = "./Engine/Assets/Textures/black_texture.png";

    //_ec->scene->registry.get<MeshComponent>(_go1).modelPath = "./Engine/Assets/PBR_test/ssphere.FBX.fbx";

    
    //_ec->scene->registry.get<MeshComponent>(_go1).material = _rc->materials[0].get();
    //_ec->scene->registry.get<MeshComponent>(_go1).texturePath = "./Engine/Assets/PBR_test/Crystal_stone_baseColor.jpg";
    //_ec->scene->registry.get<TransformComponent>(_go1).localScale = Vector3(0.05f, 0.05f, 0.05f);
    //auto mesh = _go->GetComponent<MeshComponent>();
    //GameObject* _go3 = EngineCore::instance()->scene->AddGameObject();
    //
    ////_go3->GetComponent<TransformComponent>().SetPosition(Vector3(-3, 0, 0));
    //_go3->GetComponent<MeshComponent>().UpdateMesh("./Game/Assets/DefaultModel.obj");
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


    //auto model = GameObjectHelper::CreateMesh(ecs, ServiceLocator::instance()->Get<ResourceService>()->GetUUIDFromPath("./Game/Assets/fbx/artifact.fbx"));
    //ser::Serializer& ser = ServiceLocator::instance()->Get<EngineContext>()->serializer;
    //ser.AddEcsCompMeta<GameObjectComp>();
    //ser.AddEcsCompMeta<TransformComponent>();
    //ser.AddEcsCompMeta<MeshComponent>();
    //ser.AddEcsCompMeta<LightComponent>();
    //ser.AddEcsCompMeta<EditorBillboardComponent>();

    //auto json = ser.Serialize<GameObjectComp>(*ecs);
    //std::ofstream file;
    //file.open("scene.json", std::ios::trunc);
    //file << std::setw(1) << json;
    //file.close();

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
