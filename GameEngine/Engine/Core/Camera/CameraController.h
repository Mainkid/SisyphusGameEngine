#pragma once


#include "Camera.h"
//#include "../Components/Player.h"
#include "../../../vendor/SimpleMath.h"
#include "../../../vendor/Delegates.h"

class EngineCore;
class CameraController
{
public:
    CameraController();
    CameraController(std::unique_ptr<Camera>& cam);


    DirectX::SimpleMath::Matrix GetViewMatrix();
    DirectX::SimpleMath::Matrix GetProjectionMatrix();

    void Initialize();
    void RawInput(POINT p);

    void CameraMovement (float deltaSec);

    void UpdateAspectRatio(float aspectRatio);
    

    std::unique_ptr<Camera> camera; 
    float cameraSpeed = 2.0f;
    float arm=2.0f;
    float targetArm=2.0f;
    DirectX::XMMATRIX orbitRotMat=DirectX::XMMatrixIdentity();
    int index=0;
    
};
