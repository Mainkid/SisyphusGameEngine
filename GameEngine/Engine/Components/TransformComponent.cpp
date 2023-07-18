#include "TransformComponent.h"

TransformComponent::TransformComponent(Vector3 pos, Vector3 rot, Vector3 scale)
{
    this->rotation = rot;
    this->scale = scale;
    this->position = pos;
}

Matrix& TransformComponent::ConstructTransformMatrix()
{
    Matrix result = scaleMatrix * rotationMatrix * translationMatrix;
    return result;
}

Vector3 TransformComponent::GetPosition()
{
    return position;
}

Vector3 TransformComponent::GetRotation()
{
    return rotation;
}

Vector3 TransformComponent::GetScale()
{
    return scale;
}

void TransformComponent::SetPosition(Vector3 _position)
{
    this->position = _position;
    UpdateTranslationMatrix();
}

void  TransformComponent::SetRotation(Vector3 _rotation)
{
    this->rotation = _rotation;
    UpdateRotationMatrix();
}

void TransformComponent::SetScale(Vector3 _scale)
{
    this->scale = _scale;
    UpdateScaleMatrix();
}

void TransformComponent::UpdateRotationMatrix()
{
    rotationMatrix = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(rotation);
}

void TransformComponent::UpdateTranslationMatrix()
{
    translationMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(position);
}

void TransformComponent::UpdateScaleMatrix()
{
    scaleMatrix = DirectX::SimpleMath::Matrix::CreateScale(scale);
}