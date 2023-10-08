#pragma once
#include "ResourceBase.h"
#include <d3d11.h>
#include <wrl.h>


struct Texture : public ResourceBase
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> textureSamplerState;
};