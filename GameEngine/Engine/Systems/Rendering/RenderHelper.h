#pragma once
#include "../../Core/ServiceLocator.h"

class RenderHelper {

public:
    static float GetPixelValue(int clickX, int clickY)
    {
        HardwareContext* hc = ServiceLocator::instance()->Get<HardwareContext>();
        RenderContext* rc = ServiceLocator::instance()->Get<RenderContext>();
        hc->context->CopyResource(rc->gBuffer->depthCpuTexture.Get(), rc->gBuffer->depthTexture.Get());

        D3D11_MAPPED_SUBRESOURCE ResourceDesc = {};
        hc->context->Map(rc->gBuffer->depthCpuTexture.Get(), 0, D3D11_MAP_READ, 0, &ResourceDesc);
        int const BytesPerPixel = sizeof(FLOAT);
        hc->context->Unmap(rc->gBuffer->depthCpuTexture.Get(), 0);
        if (ResourceDesc.pData)
        {
            auto res = *((float*)ResourceDesc.pData + (clickX + clickY * (rc->gBuffer->t_width)) * 4);
            return res;
        }
        return -1;
    }
    static DirectX::SimpleMath::Vector2 GetRtvResolution()
    {
        RenderContext* rc = ServiceLocator::instance()->Get<RenderContext>();
        return DirectX::SimpleMath::Vector2(rc->gBuffer->t_width, rc->gBuffer->t_height);
    }
};

