#pragma once
#include <directxmath.h>


class Camera
{
public:
    public:
    Camera();
    void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);

    const DirectX::XMMATRIX & GetViewMatrix() const;
    const DirectX::XMMATRIX & GetProjectionMatrix() const;

    const DirectX::XMVECTOR & GetPositionVector() const;
    const DirectX::XMFLOAT3 & GetPositionFloat3() const;

    const DirectX::XMVECTOR & GetRotationVector() const;
    const DirectX::XMFLOAT3 & GetRotationFloat3() const;

    void SetPosition(const DirectX::XMVECTOR & pos);
    void SetPosition(float x, float y, float z);
    void SetMatrix(const DirectX::XMMATRIX & mat);
    void AdjustPosition(const DirectX::XMVECTOR & pos);
    void AdjustPosition(float x, float y, float z);
    void SetRotation(const DirectX::XMVECTOR & rot);
    void SetRotation(float x, float y, float z);
    void AdjustRotation(const DirectX::XMVECTOR & rot);
    void AdjustRotation(float x, float y, float z);
    void AddjustTransformation(DirectX::XMMATRIX transformMat);
    void SetViewMatrix(DirectX::XMMATRIX vMat);
    
    

    void SetLookAtPos(DirectX::XMFLOAT3 lookAtPos);

    const DirectX::XMVECTOR & GetForwardVector();
    const DirectX::XMVECTOR & GetRightVector();
    const DirectX::XMVECTOR & GetLeftVector();
    const DirectX::XMVECTOR & GetBackwardVector();
    DirectX::XMMATRIX rotationRollMat=DirectX::XMMatrixIdentity();
    DirectX::XMMATRIX rotationPitchMat=DirectX::XMMatrixIdentity();
    
    DirectX::XMFLOAT3 rot;
    DirectX::XMFLOAT3 pos;
    float farZ;
private:
    void UpdateViewMatrix();
    
    DirectX::XMVECTOR posVector;
    DirectX::XMVECTOR rotVector;
    
    
    DirectX::XMMATRIX viewMatrix;
    DirectX::XMMATRIX projectionMatrix;
    

    const DirectX::XMVECTOR FORWARD_VECTOR = DirectX::XMVectorSet(0.0f,0.0f,1.0f,0.0f);
    const DirectX::XMVECTOR UP_VECTOR = DirectX::XMVectorSet(0.0f,1.0f,0.0f,0.0f);
    const DirectX::XMVECTOR BACKWARD_VECTOR = DirectX::XMVectorSet(0.0f,0.0f,-1.0f,0.0f);
    const DirectX::XMVECTOR LEFT_VECTOR = DirectX::XMVectorSet(-1.0f,0.0f,0.0f,0.0f);
    const DirectX::XMVECTOR RIGHT_VECTOR = DirectX::XMVectorSet(1.0f,0.0f,0.0f,0.0f);
    
    DirectX::XMVECTOR vec_forward;
    DirectX::XMVECTOR vec_left;
    DirectX::XMVECTOR vec_right;
    DirectX::XMVECTOR vec_backward;
    
};
