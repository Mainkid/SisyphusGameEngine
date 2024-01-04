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

	EditorBillboardComponent(const EditorBillboardComponent& ebc)
	{
		this->texture = ebc.texture;
		this->samplerState = ebc.samplerState;
		this->texturePath = ebc.texturePath;
		this->hash = ebc.hash;
	}

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState = nullptr;
	std::string texturePath;
	size_t hash = 0;

	SER_COMP(EditorBillboardComponent, 
		texturePath)
};