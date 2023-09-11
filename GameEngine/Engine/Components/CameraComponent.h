#pragma once
#include "SimpleMath.h"

using namespace DirectX::SimpleMath;
struct CameraComponent
{
	Matrix view = Matrix::Identity;
	Matrix projection = Matrix::Identity;
	float nearPlane = 0.01f;
	float farPlane = 100.0f;
	float fovDegrees = 90.0f;
	float aspectRatio = 720.0 / 1280.0f;
	float cameraSpeed = 1.0f;
	uint32_t hash = 0;
	uint32_t transformHash = 0;

	const Vector4 FORWARD_VECTOR = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const Vector4 UP_VECTOR = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const Vector4 BACKWARD_VECTOR = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const Vector4 LEFT_VECTOR = DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	const Vector4 RIGHT_VECTOR = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	Vector4 forward = FORWARD_VECTOR;
	Vector4 up = UP_VECTOR;
	Vector4 back = BACKWARD_VECTOR;
	Vector4 left = LEFT_VECTOR;
	Vector4 right = RIGHT_VECTOR;

};

namespace std
{
	template<> struct hash<CameraComponent>
	{
		using argument_type = CameraComponent;
		using result_type = std::size_t;
		result_type operator()(argument_type const& a) const
		{
			result_type const h1(std::hash<float>()(a.nearPlane));
			result_type const h2(std::hash<float>()(a.farPlane));
			return h1 * 37 + (h1 * 37 + h2) * 37;
		}
	};
}