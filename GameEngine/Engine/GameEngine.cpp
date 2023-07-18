#include <iostream>
#include "Components/GameObject.h"
#include "Core/EngineCore.h"

int main()
{
    //EngineCore* engine = new EngineCore();
    EngineCore::instance()->InitializeDirectX();
    GameObject* _go = EngineCore::instance()->scene->AddGameObject();
    _go->GetComponent<TransformComponent>().SetPosition(Vector3(-3, 0, 0));
    auto t = _go->GetComponent<TransformComponent>();
    GameObject* _go2 = EngineCore::instance()->scene->AddGameObject();
    GameObject* _go3 = EngineCore::instance()->scene->AddGameObject();
    _go3->GetComponent<TransformComponent>().SetPosition(Vector3(0, 0, -2));
    GameObject* _lightDir = EngineCore::instance()->scene->AddLight(Directional);
    GameObject* light=EngineCore::instance()->scene->AddLight(Ambient);
    light->GetComponent<LightComponent>().color = Vector4(1.0f, 1.2f, 1.0f, 0.0f);
    light->GetComponent<LightComponent>().params = Vector4(0.2f, 0.2f, 0.2f, 0.2f);
    auto res= EngineCore::instance()->scene->DestroyGameObject(_go2);
    _go2 = EngineCore::instance()->scene->AddGameObject();
    //engine->scene->Initialize();
    
    EngineCore::instance()->StartUpdateLoop();

    
    std::cout << "Hello World!\n";
}

