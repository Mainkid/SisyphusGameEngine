#pragma once
#include "GameComponent.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "../DirectXSDK/SimpleMath.h"

using namespace DirectX::SimpleMath;

class TransformComponent : public GameComponent
{
public:

	TransformComponent() = default;
	TransformComponent(Vector3 pos, Vector3 rot, Vector3 scale);
	TransformComponent(const TransformComponent& component) = default;
	Matrix ConstructTransformMatrix();
	void Initialize() {};

	Vector3 GetPosition();
	void SetPosition(Vector3 _pos);
	Vector3 GetRotation();
	Vector3 GetRotationDegrees();
	void SetRotation(Vector3 _rot);
	void SetRotationDegrees(Vector3 _rot);
	Vector3 GetScale();
	void SetScale(Vector3 _scale);

	

private:
	Vector3 position = Vector3(0, 0, 0);
	Vector3 rotation = Vector3(0, 0, 0);
	Vector3 scale = Vector3(1, 1, 1);
	Matrix translationMatrix=Matrix::Identity;
	Matrix rotationMatrix=Matrix::Identity;
	Matrix scaleMatrix=Matrix::Identity;

	void UpdateTranslationMatrix();
	void UpdateScaleMatrix();
	void UpdateRotationMatrix();
	void Render() {};

	void RadToDegrees(Vector3& vec);
	void DegreesToRad(Vector3& vec);

	
};

