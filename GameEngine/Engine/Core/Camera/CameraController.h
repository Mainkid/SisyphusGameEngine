#pragma once


#include "Camera.h"
//#include "../Components/Player.h"
#include "../DirectXSDK/SimpleMath.h"
#include "../DirectXSDK/Delegates.h"

class EngineCore;
class CameraController
{
public:
    CameraController();
    CameraController(std::unique_ptr<Camera>& cam);

    DirectX::SimpleMath::Matrix GetViewMatrix();

    DirectX::SimpleMath::Matrix GetProjectionMatrix();

    void RawInput(POINT p);

    void CameraMovement (float deltaSec);
    

    std::unique_ptr<Camera> camera; 
    float cameraSpeed = 2.0f;
    float arm=2.0f;
    float targetArm=2.0f;
    DirectX::XMMATRIX orbitRotMat=DirectX::XMMatrixIdentity();
    int index=0;
    
};
