#include <iostream>
#include "Core/EngineCore.h"


int main()
{

    //EngineCore* engine = new EngineCore();
    EngineCore::instance()->StartUp();
    EngineContext* ec = EngineCore::instance()->ec;
    auto _go = EngineCore::instance()->ec->scene->AddGameObject();
    auto _go1 = EngineCore::instance()->ec->scene->AddGameObject();
    auto _go2 = EngineCore::instance()->ec->scene->AddGameObject();
    ////_go->GetComponent<TransformComponent>().SetPosition(Vector3(-3, 0, 0));
    //auto t = _go->GetComponent<TransformComponent>();
    //
    //GameObject* _go2 = EngineCore::instance()->scene->AddGameObject();
    //_go2->SetParent(_go);
    ////_go->GetComponent<MeshComponent>().UpdateMesh("./Engine/Assets/floor.fbx");
    //auto mesh = _go->GetComponent<MeshComponent>();
    //GameObject* _go3 = EngineCore::instance()->scene->AddGameObject();
    //
    ////_go3->GetComponent<TransformComponent>().SetPosition(Vector3(-3, 0, 0));
    //_go3->GetComponent<MeshComponent>().UpdateMesh("./Engine/Assets/DefaultModel.obj");
    auto _lightDir = EngineCore::instance()->ec->scene->AddLight(LightType::Directional);
    auto light = EngineCore::instance()->ec->scene->AddLight(LightType::Ambient);
    EngineCore::instance()->ec->scene->registry.get<LightComponent>(light).color = Vector4(0.15f, 0.15f, 0.15f, 0.15f);
    auto _lightPoint = EngineCore::instance()->ec->scene->AddLight(LightType::PointLight);

    EngineCore::instance()->ec->scene->registry.get<LightComponent>(_lightPoint).paramsRadiusAndAttenuation = Vector4(3.0f, 0.0f, 0.0f, 1.0f);
    EngineCore::instance()->ec->scene->registry.get<LightComponent>(_lightPoint).color = Vector4(1, 1, 0.0f, 1.0f);
    EngineCore::instance()->ec->scene->registry.get<TransformComponent>(_lightPoint).position = Vector3(10, 10, 10);
    //_go3->SetParent(_go);

    ////GameObject* particles = EngineCore::instance()->scene->AddParticleSystem();
    ////light->GetComponent<LightComponent>().params = Vector4::Zero;
    ////auto res= EngineCore::instance()->scene->DestroyGameObject(_go2);
    ////_go2 = EngineCore::instance()->scene->AddGameObject();
    ////engine->scene->Initialize();
    
    EngineCore::instance()->StartUpdateLoop();

    
    std::cout << "Hello World!\n";
}
