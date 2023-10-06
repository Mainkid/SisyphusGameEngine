#include <iostream>
#include <uuid.hpp>
#include "Core/EngineCore.h"

#define SY_PI 3.14f
#define SY_PI2 SY_PI / 2 

int main()
{
    std::unique_ptr<Material> material = std::make_unique<Material>();
    material->albedoTexturePath = "./Engine/Assets/PBR_test/sspere_diff.png";
    material->metallicTexturePath = "./Engine/Assets/PBR_test/SS_LP_metallic.png";
    material->normalmapTexturePath = "./Engine/Assets/PBR_test/SS_LP_normal.png";
    //material->emissiveTexturePath = "./Engine/Assets/PBR_test/Crystal_stone_emissive.jpg";
    material->roughnessTexturePath = "./Engine/Assets/PBR_test/SS_LP_roughness.png";
    //material->roughnessValue = (1, 0, 0, 0);

    EngineCore::instance()->StartUp();
    auto rc=ServiceLocator::instance()->Get<RenderContext>();
    rc->materialSet.insert(std::move(material));

    auto it=rc->materialSet.begin();
    
    //EngineCore* engine = new EngineCore();
    
    EngineContext* ec = EngineCore::instance()->ec;
  /*  auto box1 = ec->scene->AddStaticBox({ -5.0f, -5.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 3.0f, 1.0f, 3.0f });
    auto box2 = ec->scene->AddDynamicBox({ -5.0f, 10.0f, 0.0f }, { SY_PI2 / 2, 0.0f, SY_PI2 / 2 });*/
    //auto _go = EngineCore::instance()->ec->scene->AddGameObject();
    auto _go1 = EngineCore::instance()->ec->scene->AddGameObject();
    //auto _go2 = EngineCore::instance()->ec->scene->AddGameObject();
    ////_go->GetComponent<TransformComponent>().SetPosition(Vector3(-3, 0, 0));
    //auto t = _go->GetComponent<TransformComponent>();
    //
    //GameObject* _go2 = EngineCore::instance()->scene->AddGameObject();
    //_go2->SetParent(_go);
    //ec->scene->registry.get<MeshComponent>(_go).modelPath = "./Engine/Assets/sphere.fbx";
    //ec->scene->registry.get<MeshComponent>(_go).texturePath = "./Engine/Assets/Textures/black_texture.png";

    ec->scene->registry.get<MeshComponent>(_go1).modelPath = "./Engine/Assets/PBR_test/ssphere.FBX.fbx";

    
    //ec->scene->registry.get<MeshComponent>(_go1).material = rc->materials[0].get();
    //ec->scene->registry.get<MeshComponent>(_go1).texturePath = "./Engine/Assets/PBR_test/Crystal_stone_baseColor.jpg";
    ec->scene->registry.get<TransformComponent>(_go1).localScale = Vector3(0.05f, 0.05f, 0.05f);
    //auto mesh = _go->GetComponent<MeshComponent>();
    //GameObject* _go3 = EngineCore::instance()->scene->AddGameObject();
    //
    ////_go3->GetComponent<TransformComponent>().SetPosition(Vector3(-3, 0, 0));
    //_go3->GetComponent<MeshComponent>().UpdateMesh("./Game/Assets/DefaultModel.obj");
    auto _lightDir = EngineCore::instance()->ec->scene->AddLight(LightType::Directional);
    auto light = EngineCore::instance()->ec->scene->AddLight(LightType::Ambient);
    EngineCore::instance()->ec->scene->registry.get<LightComponent>(light).color = Vector4(0.01f, 0.01f, 0.01f, 0.01f);
    EngineCore::instance()->ec->scene->registry.get<LightComponent>(_lightDir).color = Vector4(1.0f, 1.0f,1.0f, 1.0f);
    auto _lightPoint = EngineCore::instance()->ec->scene->AddLight(LightType::PointLight);

    EngineCore::instance()->ec->scene->registry.get<LightComponent>(_lightPoint).paramsRadiusAndAttenuation = Vector4(3.0f, 0.0f, 0.0f, 1.0f);
    EngineCore::instance()->ec->scene->registry.get<LightComponent>(_lightPoint).color = Vector4(1, 0, 1.0f, 1.0f);
    EngineCore::instance()->ec->scene->registry.get<TransformComponent>(_lightPoint).position = Vector3(3, 0, 0);

    //auto _lightPoint2 = EngineCore::instance()->ec->scene->AddLight(LightType::PointLight);

    //EngineCore::instance()->ec->scene->registry.get<LightComponent>(_lightPoint2).paramsRadiusAndAttenuation = Vector4(3.0f, 0.0f, 0.0f, 1.0f);
    //EngineCore::instance()->ec->scene->registry.get<LightComponent>(_lightPoint2).color = Vector4(1, 0, 0.0f, 3.0f);
    //EngineCore::instance()->ec->scene->registry.get<TransformComponent>(_lightPoint2).position = Vector3(3, 0, 0);
    //EngineCore::instance()->ec->scene->registry.get<LightComponent>(_lightPoint2).lightBehavior = LightBehavior::Static;
    //_go3->SetParent(_go);

    ////GameObject* particles = EngineCore::instance()->scene->AddParticleSystem();
    ////light->GetComponent<LightComponent>().params = Vector4::Zero;
    ////auto res= EngineCore::instance()->scene->DestroyGameObject(_go2);
    ////_go2 = EngineCore::instance()->scene->AddGameObject();
    ////engine->scene->Initialize();

    
    
    EngineCore::instance()->StartUpdateLoop();

    
    std::cout << "Hello World!\n";
}
