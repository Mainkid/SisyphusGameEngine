#include "CameraController.h"

#include <iostream>
#include "../EngineCore.h"


CameraController::CameraController(EngineCore* _engine)
{
    this->engine=_engine;
    this->camera=std::make_unique<Camera>();
    this->camera->SetProjectionValues(90, engine->window->GetHeight() / engine->window->GetWidth(), 0.1f, 100.0f);
}

CameraController::CameraController(std::unique_ptr<Camera>& cam,EngineCore* _engine)
{
    this->engine=_engine;
    this->camera=std::move(cam);
    //engine->wInput->RawOffsetEvent.AddRaw(this,&CameraController::RawInput);
}

void CameraController::RawInput(POINT p)
{
    
    if (engine->wInput->IsKeyDown(Keys::RightButton))
        this->camera->AdjustRotation(p.y*engine->deltaTime,p.x*engine->deltaTime,0.0f);
        
   
    std::cout<<camera->rot.x<<" "<<camera->rot.y<<" "<<camera->rot.z<<std::endl;
    std::cout<<camera->GetPositionFloat3().x<<" "<<camera->GetPositionFloat3().y<<" "<<camera->GetPositionFloat3().z<<std::endl;

}


void CameraController::CameraMovement(float deltaSec)
{
    
    using namespace DirectX;
    
      
    if (engine->wInput->IsKeyDown(Keys::W))
    {
        this->camera->AdjustPosition(this->camera->GetForwardVector()*cameraSpeed*deltaSec);
        
    }
    if (engine->wInput->IsKeyDown(Keys::A))
    {
        this->camera->AdjustPosition(this->camera->GetLeftVector()*cameraSpeed*deltaSec);
       
    }
    if (engine->wInput->IsKeyDown(Keys::S))
    {
        this->camera->AdjustPosition( this->camera->GetBackwardVector()*cameraSpeed*deltaSec);
    }
    if (engine->wInput->IsKeyDown(Keys::D))
    {
   
        this->camera->AdjustPosition(this->camera->GetRightVector()*cameraSpeed*deltaSec);
    }
    if (engine->wInput->IsKeyDown(Keys::Space))
    {
        this->camera->AdjustPosition(0.0f,cameraSpeed*deltaSec,0.0f);
    }
    if (engine->wInput->IsKeyDown(Keys::LeftShift))
    {
        this->camera->AdjustPosition(0.0f,-cameraSpeed*deltaSec,0.0f);
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

//TODO: !!! Раскоментировть строчку с делегатом