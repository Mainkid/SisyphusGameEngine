#pragma once
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include <wrl.h>
#include "../../vendor/WICTextureLoader.h"
#include <string>
#include "../Serialization/Serializable.h"

using namespace DirectX::SimpleMath;
struct EditorBillboardComponent
{
	EditorBillboardComponent()
	{
		this->texturePath = "Engine/Assets/DefaultTexture.png";
	}

	EditorBillboardComponent(std::string _texturePath)
	{
		this->texturePath = _texturePath;
	}
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState = nullptr;
	std::string texturePath;
	uint32_t hash = 0;

	SER_COMP(EditorBillboardComponent, 
		texturePath)
};