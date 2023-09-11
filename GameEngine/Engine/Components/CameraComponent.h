#pragma once
#include "SimpleMath.h"

using namespace DirectX::SimpleMath;
struct CameraComponent
{
	Matrix view = Matrix::Identity;
	Matrix projection = Matrix::Identity;
	float nearPlane = 0.01f;
	float farPlane = 100.0f;
	uint32_t hash = 0;


};