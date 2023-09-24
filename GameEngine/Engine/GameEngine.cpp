#include <iomanip>
#include <iostream>

#include "Core/EngineCore.h"

#define SY_PI 3.14f
#define SY_PI2 SY_PI / 2 

struct S
{
    union {
        DirectX::SimpleMath::Vector3 dxVec;
        physx::PxVec3 pxVec3;
    };
    S() : dxVec() {};
};

int main()
{
    //EngineCore* engine = new EngineCore();
    EngineCore::instance()->StartUp();
    EngineContext* ec = EngineCore::instance()->ec;
    auto box1 = ec->scene->AddStaticBox({ -5.0f, -5.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 3.0f, 1.0f, 3.0f });
    auto box2 = ec->scene->AddDynamicBox({ -5.0f, 10.0f, 0.0f }, { SY_PI2 / 2, 0.0f, SY_PI2 / 2 });
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
    auto _lightDir = EngineCore::instance()->ec->scene->AddLight(LightType::Directional);
    auto light = EngineCore::instance()->ec->scene->AddLight(LightType::Ambient);
    EngineCore::instance()->ec->scene->registry.get<LightComponent>(light).color = Vector4(0.15f, 0.15f, 0.15f, 0.15f);
    auto _lightPoint = EngineCore::instance()->ec->scene->AddLight(LightType::PointLight);

    EngineCore::instance()->ec->scene->registry.get<LightComponent>(_lightPoint).paramsRadiusAndAttenuation = Vector4(3.0f, 0.0f, 0.0f, 1.0f);
    EngineCore::instance()->ec->scene->registry.get<LightComponent>(_lightPoint).color = Vector4(1, 1, 0.0f, 3.0f);
    EngineCore::instance()->ec->scene->registry.get<TransformComponent>(_lightPoint).position = Vector3(3, 0, 0);

    auto _lightPoint2 = EngineCore::instance()->ec->scene->AddLight(LightType::PointLight);

    EngineCore::instance()->ec->scene->registry.get<LightComponent>(_lightPoint2).paramsRadiusAndAttenuation = Vector4(3.0f, 0.0f, 0.0f, 1.0f);
    EngineCore::instance()->ec->scene->registry.get<LightComponent>(_lightPoint2).color = Vector4(1, 0, 0.0f, 3.0f);
    EngineCore::instance()->ec->scene->registry.get<TransformComponent>(_lightPoint2).position = Vector3(3, 0, 0);
    EngineCore::instance()->ec->scene->registry.get<LightComponent>(_lightPoint2).lightBehavior = LightBehavior::Static;
    //_go3->SetParent(_go);

    ////GameObject* particles = EngineCore::instance()->scene->AddParticleSystem();
    ////light->GetComponent<LightComponent>().params = Vector4::Zero;
    ////auto res= EngineCore::instance()->scene->DestroyGameObject(_go2);
    ////_go2 = EngineCore::instance()->scene->AddGameObject();
    ////engine->scene->Initialize();
    
    EngineCore::instance()->StartUpdateLoop();

    
    std::cout << "Hello World!\n";
    

}
