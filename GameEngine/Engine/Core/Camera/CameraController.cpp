#include "CameraController.h"

#include <iostream>
#include "../EngineCore.h"


CameraController::CameraController()
{
    this->camera=std::make_unique<Camera>();
    this->camera->SetProjectionValues(90, EngineCore::instance()->window->GetWidth() / EngineCore::instance()->window->GetHeight(), 0.1f, 40.0f);
    EngineCore::instance()->wInput->RawOffsetEvent.AddRaw(this, &CameraController::RawInput);
}

CameraController::CameraController(std::unique_ptr<Camera>& cam)
{
    this->camera=std::move(cam);
    EngineCore::instance()->wInput->RawOffsetEvent.AddRaw(this,&CameraController::RawInput);
}

void CameraController::RawInput(POINT p)
{
    
    if (EngineCore::instance()->wInput->IsKeyDown(Keys::RightButton))
        this->camera->AdjustRotation(p.y* EngineCore::instance()->deltaTime,p.x* EngineCore::instance()->deltaTime,0.0f);
        
   
    std::cout<<camera->rot.x<<" "<<camera->rot.y<<" "<<camera->rot.z<<std::endl;
    std::cout<<camera->GetPositionFloat3().x<<" "<<camera->GetPositionFloat3().y<<" "<<camera->GetPositionFloat3().z<<std::endl;

}


void CameraController::CameraMovement(float deltaSec)
{
    
    using namespace DirectX;
    
      
    if (EngineCore::instance()->wInput->IsKeyDown(Keys::W))
    {
        this->camera->AdjustPosition(this->camera->GetForwardVector()*cameraSpeed*deltaSec);
        
    }
    if (EngineCore::instance()->wInput->IsKeyDown(Keys::A))
    {
        this->camera->AdjustPosition(this->camera->GetLeftVector()*cameraSpeed*deltaSec);
       
    }
    if (EngineCore::instance()->wInput->IsKeyDown(Keys::S))
    {
        this->camera->AdjustPosition( this->camera->GetBackwardVector()*cameraSpeed*deltaSec);
    }
    if (EngineCore::instance()->wInput->IsKeyDown(Keys::D))
    {
   
        this->camera->AdjustPosition(this->camera->GetRightVector()*cameraSpeed*deltaSec);
    }
    if (EngineCore::instance()->wInput->IsKeyDown(Keys::Space))
    {
        this->camera->AdjustPosition(0.0f,cameraSpeed*deltaSec,0.0f);
    }
    if (EngineCore::instance()->wInput->IsKeyDown(Keys::LeftShift))
    {
        this->camera->AdjustPosition(0.0f,-cameraSpeed*deltaSec,0.0f);
    }
    if (EngineCore::instance()->wInput->IsKeyDown(Keys::E))
    {
        for (auto& entity : EngineCore::instance()->scene->registry.view<LightComponent>())
        {
            if (EngineCore::instance()->scene->registry.get<LightComponent>(entity).lightType == Directional)
            {
                EngineCore::instance()->scene->registry.get<LightComponent>(entity).direction = camera->GetForwardVector();
            }
        }
    }
}

DirectX::SimpleMath::Matrix CameraController::GetProjectionMatrix()
{
    return camera->GetProjectionMatrix();
}

DirectX::SimpleMath::Matrix CameraController::GetViewMatrix()
{
    return camera->GetViewMatrix();
}
