#include "../../Core/EngineCore.h"
#include "RenderTarget.h"


RenderTarget::RenderTarget()
{
	renderTargetTexture = nullptr;
	renderTargetView = nullptr;
	shaderResourceView = nullptr;
	
}

bool RenderTarget::Initialize(int textureWidth, int textureHeight)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// описание render target (цели рендера)
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	//textureDesc.MiscFlags = 0;

	// Создание текстуры
	HRESULT result = EngineCore::instance()->device->CreateTexture2D(&textureDesc, NULL, renderTargetTexture.GetAddressOf());
	if (FAILED(result))
		return false;

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	// Описание render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Создание render target view.
	result = EngineCore::instance()->device->CreateRenderTargetView(renderTargetTexture.Get(), &renderTargetViewDesc, renderTargetView.GetAddressOf());
	if (FAILED(result))
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	// Описание shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Создание shader resource view.
	result = EngineCore::instance()->device->CreateShaderResourceView(renderTargetTexture.Get(), &shaderResourceViewDesc, shaderResourceView.GetAddressOf());
	if (FAILED(result))
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	result = EngineCore::instance()->device->CreateShaderResourceView(renderTargetTexture.Get(), &srvDesc,ImageSRV.GetAddressOf());

	return true;
}


void RenderTarget::Close()
{
	//Освободить ресурсы
}


void RenderTarget::SetRenderTarget(ID3D11DepthStencilView* depthStencilView)
{
	EngineCore::instance()->context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView);
}

void RenderTarget::ClearRenderTarget( ID3D11DepthStencilView* depthStencilView, D3D11_CLEAR_FLAG flags)
{
	float color[4] = { 0, 0,0, 1 };

	EngineCore::instance()->context->ClearRenderTargetView(renderTargetView.Get(), color);
	EngineCore::instance()->context->ClearDepthStencilView(depthStencilView, flags, 1.0f, 0);
}

ID3D11ShaderResourceView* RenderTarget::GetSRV()
{
	return shaderResourceView.Get();
}

ID3D11ShaderResourceView* RenderTarget::GetImageSRV()
{
	return ImageSRV.Get();
}