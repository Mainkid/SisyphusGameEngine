#pragma once
#include "SimpleMath.h"
#include "../Core/Graphics/Buffer.h"
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <wrl.h>
#include <boost/uuid/uuid.hpp>
#include <boost/functional/hash.hpp>


struct SkyboxComponent
{
	std::unique_ptr<Buffer> VertexBuffer;
	std::unique_ptr<Buffer> IndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CubeMapSRV;
	boost::uuids::uuid CubemapTexture;
	uint32_t Hash = 0;
};