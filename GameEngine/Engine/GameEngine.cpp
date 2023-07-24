#include <iostream>
#include "Components/GameObject.h"
#include "Core/EngineCore.h"


int main()
{
    



    //EngineCore* engine = new EngineCore();
    EngineCore::instance()->StartUp();
    GameObject* _go = EngineCore::instance()->scene->AddGameObject();
    _go->GetComponent<TransformComponent>().SetPosition(Vector3(-3, 0, 0));
    auto t = _go->GetComponent<TransformComponent>();
    
    GameObject* _go2 = EngineCore::instance()->scene->AddGameObject();
    _go2->SetParent(_go);
    //_go->GetComponent<MeshComponent>().UpdateMesh("./Engine/Assets/floor.fbx");
    auto mesh = _go->GetComponent<MeshComponent>();
    GameObject* _go3 = EngineCore::instance()->scene->AddGameObject();
    
    _go3->GetComponent<TransformComponent>().SetPosition(Vector3(-3, 0, 0));
    _go3->GetComponent<MeshComponent>().UpdateMesh("./Engine/Assets/DefaultModel.obj");
    GameObject* _lightDir = EngineCore::instance()->scene->AddLight(Directional);
    GameObject* light=EngineCore::instance()->scene->AddLight(Ambient);
    light->GetComponent<LightComponent>().color = Vector4(0.15f, 0.15f, 0.15f, 0.15f);
    _go3->AddChild(light);
    _go3->SetParent(_go);
    //light->GetComponent<LightComponent>().params = Vector4::Zero;
    //auto res= EngineCore::instance()->scene->DestroyGameObject(_go2);
    //_go2 = EngineCore::instance()->scene->AddGameObject();
    //engine->scene->Initialize();

    
    EngineCore::instance()->StartUpdateLoop();

    
    std::cout << "Hello World!\n";
}
