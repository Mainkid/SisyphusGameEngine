#include <iostream>
#include "Components/GameObject.h"
#include "Core/EngineCore.h"

int main()
{
    EngineCore* engine = new EngineCore();
    engine->InitializeDirectX();
    GameObject* _go = engine->scene->AddGameObject();
    _go->GetComponent<TransformComponent>().SetPosition(Vector3(-3, 0, 0));
    auto t = _go->GetComponent<TransformComponent>();
    GameObject* _go2 = engine->scene->AddGameObject();
    GameObject* _go3 = engine->scene->AddGameObject();
    _go3->GetComponent<TransformComponent>().SetPosition(Vector3(0, 0, -2));
    GameObject* _lightDir = engine->scene->AddLight(Directional);
    engine->scene->AddLight(Ambient);
    auto res=engine->scene->DestroyGameObject(_go2);
    _go2 = engine->scene->AddGameObject();
    //engine->scene->Initialize();
    
    engine->StartUpdateLoop();

    
    std::cout << "Hello World!\n";
}

