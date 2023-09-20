#pragma once
#include "SimpleMath.h"
#include "../Core/Graphics/Buffer.h"
#include <windows.h>
#include <wrl.h>
using namespace Microsoft::WRL;
struct SkyboxComponent
{
	std::unique_ptr<Buffer> vertexBuffer;
	std::unique_ptr<Buffer> indexBuffer;
	ComPtr<ID3D11ShaderResourceView> cubeMapSRV;
};