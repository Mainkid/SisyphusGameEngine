#include "ImageBasedLightingSystem.h"
#include "../../Core/ServiceLocator.h"
#include "../../Contexts/EngineContext.h"
#include "../../Contexts/HardwareContext.h"
#include "../Resources/ResourceService.h"
#include "../../Contexts/RenderContext.h"
#include "../../Components/ImageBasedLightingComponent.h"
#include "../../Components/SkyboxComponent.h"
#include "../../Events/SyImageBasedLightingUpdateEvent.h"
#include "../../Events/SySceneLoadEvent.h"
#include "../../Core/Graphics/ConstantBuffer.h"
#include "../../Scene/CameraHelper.h"

SyResult ImageBasedLightingSystem::Init()
{
	_hc = ServiceLocator::instance()->Get<HardwareContext>();
	_ec = ServiceLocator::instance()->Get<EngineContext>();
	_rc = ServiceLocator::instance()->Get<RenderContext>();
	_rs = ServiceLocator::instance()->Get<ResourceService>();

	return SyResult();
}

SyResult ImageBasedLightingSystem::Run()
{
	OPTICK_EVENT();
	if (_ecs->view<SkyboxComponent>().size() == 0)
		return SyResult();

	ID3D11ShaderResourceView* nullSrv[1] = { nullptr };
	ID3D11UnorderedAccessView* nullUav[1] = { nullptr };
	auto view = _ecs->view<ImageBasedLightingComponent>();
	entt::entity skyboxEnt = _ecs->view<SkyboxComponent>().front();
	SkyboxComponent& skybox = _ecs->get<SkyboxComponent>(skyboxEnt);
	auto skyboxSrv = skybox.SkyboxRes->textureSRV;
	auto eventView = SY_GET_THIS_FRAME_EVENT_VIEW(SyImageBasedLightingUpdateEvent);
	auto loadEventView = SY_GET_THIS_FRAME_EVENT_VIEW(SySceneLoadEvent);
	for (auto& ent : view)
	{
		ImageBasedLightingComponent& iblComp = _ecs->get<ImageBasedLightingComponent>(ent);
		if (!iblComp.IsInitialized)
		{
			iblComp.IsInitialized = true;
			InitializeResources(iblComp);
		}
	}

	if (eventView.size_hint() > 0 || loadEventView.size_hint()>0)
	{
		_rc->Viewport.Width = static_cast<float>(_hc->window->GetWidth());
		_rc->Viewport.Height = static_cast<float>(_hc->window->GetHeight());
		_rc->Viewport.TopLeftX = 0;
		_rc->Viewport.TopLeftY = 0;
		_rc->Viewport.MinDepth = 0;
		_rc->Viewport.MaxDepth = 1.0f;
		_hc->context->RSSetViewports(1, &_rc->Viewport);

		for (auto& ent : view)
		{
			entt::entity skyboxEnt = _ecs->view<SkyboxComponent>().front();
			SkyboxComponent& skybox = _ecs->get<SkyboxComponent>(skyboxEnt);

			
			ImageBasedLightingComponent& iblComp = _ecs->get<ImageBasedLightingComponent>(ent);
			CB_SimpleVector4 buffer;
			buffer.params.x = _irradianceResolution;
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			HRESULT res = _hc->context->Map(_rc->LightConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			CopyMemory(mappedResource.pData, &buffer, sizeof(CB_SimpleVector4));
			_hc->context->Unmap(_rc->LightConstBuffer->buffer.Get(), 0);
			_hc->context->CSSetConstantBuffers(0, 1, _rc->LightConstBuffer->buffer.GetAddressOf());

			
			_hc->context->ClearRenderTargetView(iblComp.IrradianceCubeMapRtv.Get(), _rc->RhData.bgColor0000);
			_hc->context->CSSetSamplers(0, 1, _rc->SamplerState.GetAddressOf());
			_hc->context->CSSetShaderResources(0, 1,skyboxSrv.GetAddressOf());
			_hc->context->CSSetUnorderedAccessViews(0, 1, iblComp.IrradianceCubeMapUav.GetAddressOf(), nullptr);
			_hc->context->CSSetShader(_rc->IrradianceMapGenerator->computeShader.Get(), nullptr, 0);
			_hc->context->Dispatch(sqrt(_irradianceResolution)+1, sqrt(_irradianceResolution)+1, 1);


			float maxMipLevel = CreateFilteredCubeMap(iblComp, skybox.SkyboxRes->Resolution);
			int threadCount = 8;

			for (int mipLevel = 0; mipLevel <= maxMipLevel; mipLevel++)
			{
				int mipWidth = static_cast<int>(skybox.SkyboxRes->Resolution) >> static_cast<int>(mipLevel);
				float roughness = mipLevel / maxMipLevel;

				CB_SimpleVector4 buffer;
				buffer.params.x = roughness;
				buffer.params.y = mipLevel;
				buffer.params.z = skybox.SkyboxRes->Resolution;

				D3D11_MAPPED_SUBRESOURCE mappedResource;
				HRESULT res = _hc->context->Map(_rc->LightConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
				CopyMemory(mappedResource.pData, &buffer, sizeof(CB_SimpleVector4));
				_hc->context->Unmap(_rc->LightConstBuffer->buffer.Get(), 0);
				_hc->context->CSSetConstantBuffers(0, 1, _rc->LightConstBuffer->buffer.GetAddressOf());

				_hc->context->CSSetSamplers(0, 1, _rc->SamplerState.GetAddressOf());
				_hc->context->CSSetShaderResources(0, 1, skyboxSrv.GetAddressOf());
				_hc->context->CSSetUnorderedAccessViews(0, 1, iblComp.FilteredEnvironmentCubeMapUavs[mipLevel].GetAddressOf(), nullptr);
				_hc->context->CSSetShader(_rc->EnvironmentPrefilter->computeShader.Get(), nullptr, 0);
				_hc->context->Dispatch(std::max(mipWidth / threadCount, 1), std::max(1, mipWidth / threadCount), 1);
			}

			buffer.params.x = _lookUpTextureResolution;
			_hc->context->Map(_rc->LightConstBuffer->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			CopyMemory(mappedResource.pData, &buffer, sizeof(CB_SimpleVector4));
			_hc->context->Unmap(_rc->LightConstBuffer->buffer.Get(), 0);
			_hc->context->CSSetConstantBuffers(0, 1, _rc->LightConstBuffer->buffer.GetAddressOf());

			_hc->context->CSSetSamplers(0, 1, _rc->SamplerState.GetAddressOf());
			_hc->context->CSSetUnorderedAccessViews(0, 1, iblComp.IblLookUpUav.GetAddressOf(), nullptr);
			_hc->context->CSSetShader(_rc->IblLookUpGenerator->computeShader.Get(), nullptr, 0);
			_hc->context->Dispatch(_lookUpTextureResolution / threadCount, _lookUpTextureResolution / threadCount, 1);

		}


	}
	
	_hc->context->CSSetShaderResources(0, 1, nullSrv);
	_hc->context->CSSetUnorderedAccessViews(0, 1, nullUav, nullptr);
	_hc->context->CSSetShader(nullptr, nullptr, 0);

	return SyResult();
}

SyResult ImageBasedLightingSystem::Destroy()
{
	return SyResult();
}

void ImageBasedLightingSystem::InitializeResources(ImageBasedLightingComponent& iblComp)
{
	
	D3D11_TEXTURE2D_DESC textureDesc_ = {};
	textureDesc_.Width = _irradianceResolution;
	textureDesc_.Height = _irradianceResolution;
	textureDesc_.MipLevels = 1;
	textureDesc_.ArraySize = 6;
	
	textureDesc_.Format = DXGI_FORMAT_R32G32B32A32_TYPELESS;
	textureDesc_.SampleDesc.Count = 1;
	textureDesc_.SampleDesc.Quality = 0;
	textureDesc_.Usage = D3D11_USAGE_DEFAULT;
	textureDesc_.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS;
	textureDesc_.CPUAccessFlags = 0;
	textureDesc_.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	HRESULT result = _hc->device->CreateTexture2D(&textureDesc_,nullptr , iblComp.IrradianceCubeMapTex.GetAddressOf());

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = 1;
	
	result = _hc->device->CreateShaderResourceView(iblComp.IrradianceCubeMapTex.Get(), &srvDesc, iblComp.IrradianceCubeMapSrv.GetAddressOf());

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
	uavDesc.Texture2DArray.ArraySize = 6;
	uavDesc.Texture2DArray.FirstArraySlice = 0;
	uavDesc.Texture2DArray.MipSlice = 0;

	result = _hc->device->CreateUnorderedAccessView(iblComp.IrradianceCubeMapTex.Get(), &uavDesc, iblComp.IrradianceCubeMapUav.GetAddressOf());

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.FirstArraySlice = 0;
	rtvDesc.Texture2DArray.MipSlice = 0;
	rtvDesc.Texture2DArray.ArraySize = 6;

	result = _hc->device->CreateRenderTargetView(iblComp.IrradianceCubeMapTex.Get(), &rtvDesc, iblComp.IrradianceCubeMapRtv.GetAddressOf());

	float bgcolor[] = { 1.0f,1.0f,1.0f,1.0f };
	_hc->context->ClearRenderTargetView(iblComp.IrradianceCubeMapRtv.Get(), bgcolor);





	

}

float ImageBasedLightingSystem::CreateFilteredCubeMap(ImageBasedLightingComponent& iblComp, int cubeMapResolution)
{

	if (iblComp.FilteredEnvironmentCubeMapTex.Get() != nullptr)
		iblComp.FilteredEnvironmentCubeMapTex.Get()->Release();

	if (iblComp.FilteredEnvironmentSrv.Get() != nullptr)
		iblComp.FilteredEnvironmentSrv.Get()->Release();

	if (iblComp.IblLookUpTexture.Get() != nullptr)
		iblComp.IblLookUpTexture.Get()->Release();

	if (iblComp.IblLookUpUav.Get() != nullptr)
		iblComp.IblLookUpUav.Get()->Release();

	if (iblComp.IblLookUpSrv.Get() != nullptr)
		iblComp.IblLookUpSrv.Get()->Release();


	
	D3D11_TEXTURE2D_DESC textureDesc2_ = {};
	textureDesc2_.Width = cubeMapResolution;
	textureDesc2_.Height = cubeMapResolution;
	textureDesc2_.MipLevels = log2(cubeMapResolution)+1;
	textureDesc2_.ArraySize = 6;

	textureDesc2_.Format = DXGI_FORMAT_R32G32B32A32_TYPELESS;
	textureDesc2_.SampleDesc.Count = 1;
	textureDesc2_.SampleDesc.Quality = 0;
	textureDesc2_.Usage = D3D11_USAGE_DEFAULT;
	textureDesc2_.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS;
	textureDesc2_.CPUAccessFlags = 0;
	textureDesc2_.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;



	auto result = _hc->device->CreateTexture2D(&textureDesc2_, nullptr, iblComp.FilteredEnvironmentCubeMapTex.GetAddressOf());

	D3D11_TEXTURE2D_DESC textureDesc3_ = {};
	textureDesc3_.Width = _lookUpTextureResolution;
	textureDesc3_.Height = _lookUpTextureResolution;
	textureDesc3_.MipLevels = 1;
	textureDesc3_.ArraySize = 1;

	textureDesc3_.Format = DXGI_FORMAT_R32G32B32A32_TYPELESS;
	textureDesc3_.SampleDesc.Count = 1;
	textureDesc3_.SampleDesc.Quality = 0;
	textureDesc3_.Usage = D3D11_USAGE_DEFAULT;
	textureDesc3_.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS;
	textureDesc3_.CPUAccessFlags = 0;
	textureDesc3_.MiscFlags = 0;




	result = _hc->device->CreateTexture2D(&textureDesc3_, nullptr, iblComp.IblLookUpTexture.GetAddressOf());

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;
	result = _hc->device->CreateUnorderedAccessView(iblComp.IblLookUpTexture.Get(), &uavDesc, iblComp.IblLookUpUav.GetAddressOf());

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = 1;

	result = _hc->device->CreateShaderResourceView(iblComp.IblLookUpTexture.Get(), &srvDesc, iblComp.IblLookUpSrv.GetAddressOf());


	iblComp.FilteredEnvironmentCubeMapUavs.clear();

	for (int i = 0; i <= log2(cubeMapResolution); i++)
	{
		iblComp.FilteredEnvironmentCubeMapUavs.push_back(nullptr);

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
		uavDesc.Texture2DArray.ArraySize = 6;
		uavDesc.Texture2DArray.FirstArraySlice = 0;
		uavDesc.Texture2DArray.MipSlice = i;
		result = _hc->device->CreateUnorderedAccessView(iblComp.FilteredEnvironmentCubeMapTex.Get(), &uavDesc, iblComp.FilteredEnvironmentCubeMapUavs[i].GetAddressOf());

	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc2 = {};
	srvDesc2.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc2.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc2.TextureCube.MostDetailedMip = 0;
	srvDesc2.TextureCube.MipLevels = log2(cubeMapResolution)+1;

	result = _hc->device->CreateShaderResourceView(iblComp.FilteredEnvironmentCubeMapTex.Get(), &srvDesc2, iblComp.FilteredEnvironmentSrv.GetAddressOf());


	return log2(cubeMapResolution);
}
