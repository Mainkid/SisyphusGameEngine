#include "Camera.h"

#include "SimpleMath.h"


Camera::Camera()
{
    this->pos = DirectX::XMFLOAT3(0.0f,0.0f,0.0f);
    this->posVector = XMLoadFloat3(&this->pos);
    this->rot = DirectX::XMFLOAT3(0.0f,0.0f,0.0f);
    this->rotVector = XMLoadFloat3(&this->rot);
    SetPosition(0.0f, 0.0f, 0.0f);
    SetLookAtPos(DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f));
    UpdateViewMatrix();
    //SetProjectionValues(90, 1, 0.01f, 1000.0f);
    
}

void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
    this->farZ=farZ;
    float fovRadians = (fovDegrees / 360.0f) * DirectX::XM_2PI;
    this->projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fovRadians,aspectRatio,nearZ,farZ);
}

const DirectX::XMMATRIX& Camera::GetViewMatrix() const
{
    return this->viewMatrix;
}

const DirectX::XMMATRIX& Camera::GetProjectionMatrix() const
{
    return this->projectionMatrix;
}

const DirectX::XMVECTOR& Camera::GetPositionVector() const
{
    return this->posVector;
}

const DirectX::XMFLOAT3& Camera::GetPositionFloat3() const
{
    return this->pos;
}

const DirectX::XMVECTOR& Camera::GetRotationVector() const
{
    return this->rotVector;
}

const DirectX::XMFLOAT3& Camera::GetRotationFloat3() const
{
    return this->rot;
}

void Camera::SetPosition(const DirectX::XMVECTOR& pos)
{
    XMStoreFloat3(&this->pos,pos);
    this->posVector=pos;
    this->UpdateViewMatrix();
}

void Camera::SetMatrix(const DirectX::XMMATRIX& mat)
{
    this->viewMatrix=mat;
}



void Camera::SetPosition(float x, float y, float z)
{
    this->pos = DirectX::XMFLOAT3(x,y,z);
    this->posVector = DirectX::XMLoadFloat3(&this->pos);
    this->UpdateViewMatrix();
}

void Camera::AdjustPosition(float x, float y, float z)
{
     this->pos.x +=x;
     this->pos.y +=y;
     this->pos.z +=z;
     this->posVector = XMLoadFloat3(&this->pos);
     this->UpdateViewMatrix();
}

void Camera::AdjustPosition(const DirectX::XMVECTOR& pos)
{
    using namespace DirectX;
    this->posVector += pos;
    DirectX::XMStoreFloat3(&this->pos,this->posVector);
    this->UpdateViewMatrix();
}

void Camera::SetRotation(float x, float y, float z)
{
    this->rot = DirectX::XMFLOAT3(x,y,z);
    this->rotVector=DirectX::XMLoadFloat3(&this->rot);
    this->UpdateViewMatrix();
}

void Camera::SetRotation(const DirectX::XMVECTOR& rot)
{
    this->rotVector = rot;
    XMStoreFloat3(&this->rot,rot);
    this->UpdateViewMatrix();
}

void Camera::AdjustRotation(float x, float y, float z)
{
    this->rot.x +=x;
    this->rot.y +=y;
    this->rot.z +=z;
    this->rotVector = XMLoadFloat3(&this->rot);
    
    this->UpdateViewMatrix();
}

void Camera::AdjustRotation(const DirectX::XMVECTOR& rot)
{
    using namespace DirectX;
    this->rotVector += rot;
    XMStoreFloat3(&this->rot,this->rotVector);
    this->UpdateViewMatrix();
    
}


void Camera::UpdateViewMatrix()
{
    using namespace DirectX;
    
    DirectX::XMMATRIX camRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(
        this->rot.x,this->rot.y,this->rot.z);

    DirectX::XMVECTOR camTarget = DirectX::XMVector3TransformCoord(
        this->FORWARD_VECTOR,camRotationMatrix);
   
    camTarget += this->posVector;
    DirectX::SimpleMath::Vector3 cT=camTarget;
    DirectX::SimpleMath::Vector3 pV=this->posVector;
    DirectX::XMVECTOR upDir = DirectX::XMVector3TransformCoord(this->UP_VECTOR,camRotationMatrix);

    this->viewMatrix = DirectX::XMMatrixLookAtLH(this->posVector,camTarget,upDir);

    XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y,this->rot.z);
    this->vec_forward = XMVector3TransformCoord(this->FORWARD_VECTOR,vecRotationMatrix);
    this->vec_backward = XMVector3TransformCoord(this->BACKWARD_VECTOR,vecRotationMatrix);
    this->vec_left = XMVector3TransformCoord(this->LEFT_VECTOR,vecRotationMatrix);
    this->vec_right = XMVector3TransformCoord(this->RIGHT_VECTOR,vecRotationMatrix);
    
}





void Camera::SetViewMatrix(DirectX::XMMATRIX vMat)
{
    viewMatrix=vMat;
}


void Camera::SetLookAtPos(DirectX::XMFLOAT3 lookAtPos)
{
    if (lookAtPos.x==this->pos.x && lookAtPos.y ==this->pos.y && lookAtPos.z == this->pos.z)
        return;

    lookAtPos.x=lookAtPos.x-this->pos.x;
    lookAtPos.y=lookAtPos.y-this->pos.y;
    lookAtPos.z=lookAtPos.z-this->pos.z ;

    float pitch= 0.0f;
    if (lookAtPos.y !=0.0f)
    {
        const float distance=sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z*lookAtPos.z);
        pitch= atan(lookAtPos.y/distance);
    }

    float yaw=0.0f;
    if (lookAtPos.x != 0.0f)
    {
        yaw = atan(lookAtPos.x/lookAtPos.z);
    }

    if (lookAtPos.z >0)
        yaw+= DirectX::XM_PI;

    this->SetRotation(pitch,yaw,0.0f);
    //rotationRollMat=DirectX::XMMatrixRotationRollPitchYaw(pitch,0.0f,0.0f);
    //rotationPitchMat=DirectX::XMMatrixRotationRollPitchYaw(0.0f,pitch,0.0f);
}


void Camera::AddjustTransformation(DirectX::XMMATRIX transformMat)
{
    viewMatrix*=transformMat;
}

const DirectX::XMVECTOR& Camera::GetForwardVector() 
{
    return this->vec_forward;
}

const DirectX::XMVECTOR& Camera::GetRightVector()
{
    return this->vec_right;
}

const DirectX::XMVECTOR& Camera::GetBackwardVector()
{
    return this->vec_backward;
}

const DirectX::XMVECTOR& Camera::GetLeftVector()
{
    return this->vec_left;
}












