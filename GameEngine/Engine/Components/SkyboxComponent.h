#pragma once
#include "SimpleMath.h"
#include "../Core/Graphics/Buffer.h"
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <wrl.h>
using namespace Microsoft::WRL;
struct SkyboxComponent
{
	std::unique_ptr<Buffer> vertexBuffer;
	std::unique_ptr<Buffer> indexBuffer;
	ComPtr<ID3D11ShaderResourceView> cubeMapSRV;
};