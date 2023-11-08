#include "HbaoRenderSystem.h"
#include "../../Core/ServiceLocator.h"
#include "../../EngineContext.h"
#include "../../HardwareContext.h"
#include "../RenderContext.h"
#include "../../Components/CameraComponent.h"
#include "../../Components/TransformComponent.h"
#include "../../Scene/CameraHelper.h"

SyResult HbaoRenderSystem::Init()
{
	_hc = ServiceLocator::instance()->Get<HardwareContext>();
	_ec = ServiceLocator::instance()->Get<EngineContext>();
	_rc = ServiceLocator::instance()->Get<RenderContext>();

	_rc->HbaoParams.Radius = 2.0f;
	_rc->HbaoParams.Bias = 0.1f;
	_rc->HbaoParams.PowerExponent = 2.0f;
	_rc->HbaoParams.Blur.Enable = true;
	_rc->HbaoParams.Blur.Radius = GFSDK_SSAO_BLUR_RADIUS_4;
	_rc->HbaoParams.Blur.Sharpness = 16.0f;

	return SyResult();
}

SyResult HbaoRenderSystem::Run()
{
	auto [camera, cameraTf] = CameraHelper::Find(_ecs);

	_rc->Input.DepthData.DepthTextureType = GFSDK_SSAO_HARDWARE_DEPTHS;
	_rc->Input.DepthData.pFullResDepthTextureSRV = _hc->depthStencilSrv.Get();
	_rc->Input.DepthData.ProjectionMatrix.Data = GFSDK_SSAO_Float4x4(&camera.projection._11);
	_rc->Input.DepthData.ProjectionMatrix.Layout = GFSDK_SSAO_ROW_MAJOR_ORDER;
	_rc->Input.DepthData.MetersToViewSpaceUnits = 1.0;

	GFSDK_SSAO_Output_D3D11 Output;
	Output.pRenderTargetView = _rc->HbaoRtv.Get();
	Output.Blend.Mode = GFSDK_SSAO_OVERWRITE_RGB;

	_rc->status = _rc->pAOContext->RenderAO(_hc->context.Get(), _rc->Input, _rc->HbaoParams, Output);
	assert(_rc->status == GFSDK_SSAO_OK);

	return SyResult();
}

SyResult HbaoRenderSystem::Destroy()
{
	return SyResult();
}
