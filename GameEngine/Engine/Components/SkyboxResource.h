#pragma once
#include "ResourceBase.h"
#include <d3d11.h>
#include <wrl.h>


struct SkyboxResource : public ResourceBase
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> cubemapTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV;
	uint32_t Resolution;
};