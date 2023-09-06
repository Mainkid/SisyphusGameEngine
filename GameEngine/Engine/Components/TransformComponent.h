#pragma once
#include "SimpleMath.h"


using namespace DirectX::SimpleMath;
struct TransformComponent
{
	TransformComponent() = default;
	TransformComponent(Vector3 _pos, Vector3 _rot, Vector3 _scale)
	{
		this->position = _pos;
		this->rotation = _rot;
		this->scale = _scale;
	}
	Vector3 position = Vector3(0, 0, 0);
	Vector3 prevPos = Vector3(0, 0, 0);
	Vector3 rotation = Vector3(0, 0, 0);
	Vector3 prevRot = Vector3(0, 0, 0);
	Vector3 scale = Vector3(1, 1, 1);
	Vector3 prevScale = Vector3(1, 1, 1);
	Matrix translationMatrix=Matrix::Identity;
	Matrix rotationMatrix=Matrix::Identity;
	Matrix scaleMatrix=Matrix::Identity;
	
};

