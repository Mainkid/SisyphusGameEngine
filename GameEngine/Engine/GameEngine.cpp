#include <iostream>
#include "Core/EngineCore.h"
#include "Scene/GameObjectHelper.h"

#define SY_PI 3.14f
#define SY_PI2 SY_PI / 2 

int main()
{

    //EngineCore* engine = new EngineCore();
    EngineCore::instance()->StartUp();
    entt::registry* ecs = &ServiceLocator::instance()->Get<EngineContext>()->ecs;
    auto box1 = GameObjectHelper::CreateStaticBox(ecs, { -5.0f, -5.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 3.0f, 1.0f, 3.0f });
    auto box2 = GameObjectHelper::CreateDynamicBox(ecs, { -5.0f, 10.0f, 0.0f }, { SY_PI2 / 2, 0.0f, SY_PI2 / 2 });
    //auto _go = EngineCore::instance()->ec->scene->AddGameObject();
    //auto _go1 = EngineCore::instance()->ec->scene->AddGameObject();
    //auto _go2 = EngineCore::instance()->ec->scene->AddGameObject();
    ////_go->GetComponent<TransformComponent>().SetPosition(Vector3(-3, 0, 0));
    //auto t = _go->GetComponent<TransformComponent>();
    //
    //GameObject* _go2 = EngineCore::instance()->scene->AddGameObject();
    //_go2->SetParent(_go);
    //EngineCore::instance()->ec->scene->registry.get<MeshComponent>(_go).modelPath = "./Engine/Assets/sphere.fbx";
    //auto mesh = _go->GetComponent<MeshComponent>();
    //GameObject* _go3 = EngineCore::instance()->scene->AddGameObject();
    //
    ////_go3->GetComponent<TransformComponent>().SetPosition(Vector3(-3, 0, 0));
    //_go3->GetComponent<MeshComponent>().UpdateMesh("./Game/Assets/DefaultModel.obj");
    auto lightDir = GameObjectHelper::CreateLight(ecs, LightType::Directional);

    auto light = GameObjectHelper::CreateLight(ecs, LightType::Ambient);
    ecs->get<LightComponent>(light).color = Vector4(0.15f, 0.15f, 0.15f, 0.15f);

    auto lightPoint = GameObjectHelper::CreateLight(ecs, LightType::PointLight);
    ecs->get<LightComponent>(lightPoint).paramsRadiusAndAttenuation = Vector4(3.0f, 0.0f, 0.0f, 1.0f);
    ecs->get<LightComponent>(lightPoint).color = Vector4(1, 1, 0.0f, 3.0f);
    ecs->get<TransformComponent>(lightPoint).position = Vector3(3, 0, 0);

    auto lightPoint2 = GameObjectHelper::CreateLight(ecs, LightType::PointLight);

    ecs->get<LightComponent>(lightPoint2).paramsRadiusAndAttenuation = Vector4(3.0f, 0.0f, 0.0f, 1.0f);
    ecs->get<LightComponent>(lightPoint2).color = Vector4(1, 0, 0.0f, 3.0f);
    ecs->get<TransformComponent>(lightPoint2).position = Vector3(3, 0, 0);
    ecs->get<LightComponent>(lightPoint2).lightBehavior = LightBehavior::Static;
    //_go3->SetParent(_go);

    ////GameObject* particles = EngineCore::instance()->scene->AddParticleSystem();
    ////light->GetComponent<LightComponent>().params = Vector4::Zero;
    ////auto res= EngineCore::instance()->scene->DestroyGameObject(_go2);
    ////_go2 = EngineCore::instance()->scene->AddGameObject();
    ////engine->scene->Initialize();
    
    EngineCore::instance()->Update();
    EngineCore::instance()->ShutDown();
    
    std::cout << "Hello World!\n";

    
}
