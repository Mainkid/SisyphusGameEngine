#pragma once
#include "../../Core/ServiceLocator.h"
#include "../HardwareContext.h"
#include "../Rendering/RenderContext.h"
#include "SimpleMath.h"

#define MAX_RESOURCES 20

class RenderHelper
{
public:
	static Vector4 GetPixelValue(int clickX, int clickY)
	{
		HardwareContext* hc = ServiceLocator::instance()->Get<HardwareContext>();
		RenderContext* rc = ServiceLocator::instance()->Get<RenderContext>();
		hc->context->CopyResource(rc->GBuffer->_idCpuTexture.Get(), rc->GBuffer->_idTexture.Get());

		D3D11_MAPPED_SUBRESOURCE ResourceDesc = {};
		hc->context->Map(rc->GBuffer->_idCpuTexture.Get(), 0, D3D11_MAP_READ, 0, &ResourceDesc);
		const int BytesPerPixel = sizeof(FLOAT);
		hc->context->Unmap(rc->GBuffer->_idCpuTexture.Get(), 0);
		if (ResourceDesc.pData)
		{
			int res = *((float*)ResourceDesc.pData + (clickX + clickY * (rc->GBuffer->_width)) * 4);
			int res2 = *((float*)ResourceDesc.pData + (clickX + clickY * (rc->GBuffer->_width)) * 4 + 1);
			int res3 = *((float*)ResourceDesc.pData + (clickX + clickY * (rc->GBuffer->_width)) * 4 + 2);
			int res4 = *((float*)ResourceDesc.pData + (clickX + clickY * (rc->GBuffer->_width)) * 4 + 3);
			return DirectX::SimpleMath::Vector4(res, res2, res3, res4);
		}
		return DirectX::SimpleMath::Vector4(-1, -1, -1, -1);
	}

	static DirectX::SimpleMath::Vector2 GetRtvResolution()
	{
		RenderContext* rc = ServiceLocator::instance()->Get<RenderContext>();
		return DirectX::SimpleMath::Vector2(rc->GBuffer->_width, rc->GBuffer->_height);
	}


	


};
