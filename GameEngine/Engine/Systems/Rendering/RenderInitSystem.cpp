#include "RenderInitSystem.h"
#include "../../Core/Graphics/ConstantBuffer.h"
#include "RenderContext.h"
#include "../../../vendor/WICTextureLoader.h"
#include "../EngineContext.h"
#include "../HardwareContext.h"
#include "../../Core/ServiceLocator.h"
#include "../../Tools/ImageLoader.h"
#include "../MeshLoader.h"

SyResult RenderInitSystem::Init()
{
	ServiceLocator::instance()->Register<RenderContext>();

	_hc = ServiceLocator::instance()->Get<HardwareContext>();
	_ec = ServiceLocator::instance()->Get<EngineContext>();
	_rc = ServiceLocator::instance()->Get<RenderContext>();
	_rc->CubeMesh = MeshLoader::LoadSimpleMesh("./Engine/Assets/Resources/Cube.fbx");

	using namespace DirectX::SimpleMath;
	{
		std::vector<int> quadIndex = {3, 1, 0, 2, 1, 3};
		std::vector<Vector4> quadCoords = {
			Vector4(-1.0f, -1.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f),
			Vector4(1.0f, -1.0f, 0.0f, 1.0f), Vector4(1.0f, 1.0f, 0.0f, 1.0f),
			Vector4(1.0f, 1.0f, 0.0f, 1.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f),
			Vector4(-1.0f, 1.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, 1.0f)
		};

		_rc->IndexQuadBuffer = std::make_unique<Buffer>(_hc->device.Get());
		_rc->IndexQuadBuffer->Initialize(quadIndex);
		_rc->VertexQuadBuffer = std::make_unique<Buffer>(_hc->device.Get());
		_rc->VertexQuadBuffer->Initialize(quadCoords);
	}


	CreateBuffers();

	_rc->Rtvs[0] = _rc->GBuffer->DiffuseRtv.Get();
	_rc->Rtvs[1] = _rc->GBuffer->MetallicRtv.Get();
	_rc->Rtvs[2] = _rc->GBuffer->SpecularRtv.Get();
	_rc->Rtvs[3] = _rc->GBuffer->EmissiveRtv.Get();
	_rc->Rtvs[4] = _rc->GBuffer->NormalRtv.Get();
	_rc->Rtvs[5] = _rc->GBuffer->PositionRtv.Get();
	_rc->Rtvs[6] = _rc->GBuffer->IdRtv.Get();
	//_rc->Rtvs[4] = _rc->GBuffer->SpecularRtv.Get();

	_rc->EditorBillboardRtvs[0] = _rc->GBuffer->HdrBufferRtv.Get();
	_rc->EditorBillboardRtvs[1] = _rc->GBuffer->IdRtv.Get();

	CreateTextures();

	CreateSamplers();

	CreateDepthAndStencils();

	CreateBlendStates();

	CreateShaderResourceViewsAndRenderTargets();

	CreateRasterizerStates();


	_rc->Viewport.Width = _hc->window->GetWidth();
	_rc->Viewport.Height = _hc->window->GetHeight();
	_rc->Viewport.TopLeftX = 0;
	_rc->Viewport.TopLeftY = 0;
	_rc->Viewport.MinDepth = 0;
	_rc->Viewport.MaxDepth = 1.0f;

	_hc->context->RSSetViewports(1, &_rc->Viewport);

	InitSkybox();

	CompileShaders();


	SY_LOG_CORE(SY_LOGLEVEL_INFO, "RenderInit system initialization successful. ");
	return SyResult();
}

SyResult RenderInitSystem::Run()
{
	return SyResult();
}

SyResult RenderInitSystem::Destroy()
{
	SY_LOG_CORE(SY_LOGLEVEL_INFO, "RenderInit system destruction successful. ");
	return SyResult();
}

void RenderInitSystem::InitSkybox() const
{
	int resolution = 512;
	D3D11_TEXTURE2D_DESC textureDesc_ = {};
	textureDesc_.Width = resolution;
	textureDesc_.Height = resolution;
	textureDesc_.MipLevels = 1;
	textureDesc_.ArraySize = 6;

	textureDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	textureDesc_.SampleDesc.Count = 1;
	textureDesc_.SampleDesc.Quality = 0;
	textureDesc_.Usage = D3D11_USAGE_DEFAULT;
	textureDesc_.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc_.CPUAccessFlags = 0;
	textureDesc_.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;


	D3D11_SUBRESOURCE_DATA data[6];
	for (int i = 0; i < 6; i++)
	{
		std::string file = "./Engine/Assets/SkyBox/skybox_";
		file += std::to_string(i + 1);
		file += ".png";
		int width = 0;
		int height = 0;
		data[i].pSysMem = ImageLoader::LoadImageFromFile(file.c_str(), &width, &height);
		data[i].SysMemPitch = sizeof(char) * width * 4;
		data[i].SysMemSlicePitch = 0;
	}
	HRESULT result = _hc->device->CreateTexture2D(&textureDesc_, data, _rc->SkyboxTexture.GetAddressOf());

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc_.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	result = _hc->device->CreateShaderResourceView(_rc->SkyboxTexture.Get(), &srvDesc, _rc->SkyboxSRV.GetAddressOf());
}

void RenderInitSystem::CreateTextures() const
{
	D3D11_TEXTURE2D_DESC shadowMapDesc;
	ZeroMemory(&shadowMapDesc, sizeof(D3D11_TEXTURE2D_DESC));
	shadowMapDesc.Width = _rc->ShadowmapWidth;
	shadowMapDesc.Height = _rc->ShadowmapHeight;
	shadowMapDesc.MipLevels = 1;
	shadowMapDesc.ArraySize = 4;
	shadowMapDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowMapDesc.SampleDesc.Count = 1;
	shadowMapDesc.SampleDesc.Quality = 0;
	shadowMapDesc.Usage = D3D11_USAGE_DEFAULT;
	shadowMapDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowMapDesc.CPUAccessFlags = 0;
	shadowMapDesc.MiscFlags = 0;

	HRESULT hr = _hc->device->CreateTexture2D(&shadowMapDesc, nullptr, &_rc->Texture);


	D3D11_TEXTURE2D_DESC shadowMapDesc_;
	ZeroMemory(&shadowMapDesc_, sizeof(D3D11_TEXTURE2D_DESC));
	shadowMapDesc_.Width = _hc->window->GetWidth();
	shadowMapDesc_.Height = _hc->window->GetHeight();
	shadowMapDesc_.MipLevels = 1;
	shadowMapDesc_.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowMapDesc_.SampleDesc.Count = 1;
	shadowMapDesc_.ArraySize = 1;
	shadowMapDesc_.SampleDesc.Quality = 0;
	shadowMapDesc_.Usage = D3D11_USAGE_DEFAULT;
	shadowMapDesc_.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	shadowMapDesc_.CPUAccessFlags = 0;
	shadowMapDesc_.MiscFlags = 0;

	hr = _hc->device->CreateTexture2D(&shadowMapDesc_, nullptr, &_rc->TexturePCF);

	D3D11_TEXTURE2D_DESC textureDesc;
	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	textureDesc.Width = _rc->ShadowmapWidth;
	textureDesc.Height = _rc->ShadowmapHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 4;
	textureDesc.Format = DXGI_FORMAT_R32G32_TYPELESS;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the render target texture.
	auto result = _hc->device->CreateTexture2D(&textureDesc, NULL, &_rc->MRenderTargetTexture);
	result = _hc->device->CreateTexture2D(&textureDesc, NULL, &_rc->MBluredShadowTexture);

	D3D11_TEXTURE2D_DESC desc2;
	ZeroMemory(&desc2, sizeof(desc2));
	desc2.Width = _hc->window->GetWidth();
	desc2.Height = _hc->window->GetHeight();
	desc2.MipLevels = 1;
	desc2.ArraySize = 1;
	desc2.Format = DXGI_FORMAT_R32_FLOAT;
	desc2.SampleDesc.Count = 1;
	desc2.Usage = D3D11_USAGE_DEFAULT;
	desc2.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc2.CPUAccessFlags = 0;

	result = _hc->device->CreateTexture2D(&desc2, NULL, _rc->EntityIdTexture.GetAddressOf());
}

void RenderInitSystem::CompileShaders() const
{
	_rc->OpaqueShader = std::make_unique<Shader>();
	_rc->OpaqueShader->Initialize(L"./Engine/Assets/Shaders/OpaqueShader.hlsl",
	                              COMPILE_VERTEX | COMPILE_PIXEL,
	                              USE_POSITION | USE_NORMAL | USE_COLOR | USE_TANGENT_BITANGENT);

	_rc->DirLightShader = std::make_unique<Shader>();
	_rc->DirLightShader->Initialize(L"./Engine/Assets/Shaders/LightShader.hlsl",
	                                COMPILE_VERTEX | COMPILE_PIXEL, USE_POSITION | USE_COLOR, "VSMain",
	                                "PS_Directional");

	_rc->AmbientLightShader = std::make_unique<Shader>();
	_rc->AmbientLightShader->Initialize(L"./Engine/Assets/Shaders/LightShader.hlsl",
	                                    COMPILE_VERTEX | COMPILE_PIXEL, USE_POSITION | USE_COLOR, "VSMain",
	                                    "PS_Ambient");

	_rc->PointLightShader = std::make_unique<Shader>();
	_rc->PointLightShader->Initialize(L"./Engine/Assets/Shaders/PointLightShader.hlsl",
	                                  COMPILE_VERTEX | COMPILE_PIXEL, USE_POSITION | USE_COLOR, "VSMain",
	                                  "PS_PointLight");

	_rc->SpotLightShader = std::make_unique<Shader>();
	_rc->SpotLightShader->Initialize(L"./Engine/Assets/Shaders/SpotLightShader.hlsl",
	                                 COMPILE_VERTEX | COMPILE_PIXEL, USE_POSITION | USE_COLOR, "VSMain",
	                                 "PS_SpotLight");

	_rc->StencilPassShader = std::make_unique<Shader>();
	_rc->StencilPassShader->Initialize(L"./Engine/Assets/Shaders/StencilPassShader.hlsl",
	                                   COMPILE_VERTEX | COMPILE_PIXEL, USE_POSITION | USE_COLOR | USE_NORMAL);

	_rc->ShadowShader = std::make_unique<Shader>();
	_rc->ShadowShader->Initialize(L"./Engine/Assets/Shaders/ShadowShader.hlsl",
	                              COMPILE_VERTEX | COMPILE_GEOM | COMPILE_PIXEL, USE_POSITION | USE_COLOR | USE_NORMAL,
	                              "DepthVertexShader", "PS_Main");

	_rc->ShadowPointLightShader = std::make_unique<Shader>();
	_rc->ShadowPointLightShader->Initialize(L"./Engine/Assets/Shaders/ShadowPointlightShader.hlsl",
	                                        COMPILE_VERTEX | COMPILE_GEOM | COMPILE_PIXEL,
	                                        USE_POSITION | USE_COLOR | USE_NORMAL, "DepthVertexShader", "PSMain");

	_rc->BillboardShader = std::make_unique<Shader>();
	_rc->BillboardShader->Initialize(L"./Engine/Assets/Shaders/SpriteBillboardShader.hlsl",
	                                 COMPILE_VERTEX | COMPILE_PIXEL, USE_POSITION | USE_COLOR | USE_NORMAL);

	_rc->ShadowMapGenerator = std::make_unique<Shader>();
	_rc->ShadowMapGenerator->Initialize(L"./Engine/Assets/Shaders/ShadowMapGenerator.hlsl",
	                                    COMPILE_VERTEX | COMPILE_PIXEL, USE_POSITION | USE_COLOR, "VSMain",
	                                    "PS_ShadowMapGenerator");

	_rc->ShadowMapPointLightGenerator = std::make_unique<Shader>();
	_rc->ShadowMapPointLightGenerator->Initialize(L"./Engine/Assets/Shaders/ShadowPointLightGenerator.hlsl",
	                                              COMPILE_VERTEX | COMPILE_PIXEL, USE_POSITION | USE_COLOR, "VS", "PS");


	_rc->SkyBoxShader = std::make_unique<Shader>();
	_rc->SkyBoxShader->Initialize(L"./Engine/Assets/Shaders/Skybox.hlsl",
	                              COMPILE_VERTEX | COMPILE_PIXEL, USE_POSITION | USE_COLOR, "VS", "PS");

	_rc->ToneMapper = std::make_unique<Shader>();
	_rc->ToneMapper->Initialize(L"./Engine/Assets/Shaders/ToneMapping.hlsl",
	                            COMPILE_VERTEX | COMPILE_PIXEL, USE_POSITION | USE_COLOR, "VSMain", "PSMain");

	_rc->EditorGridRenderer = std::make_unique<Shader>();
	_rc->EditorGridRenderer->Initialize(L"./Engine/Assets/Shaders/EditorGridShader.hlsl",
	                                    COMPILE_VERTEX | COMPILE_PIXEL, USE_POSITION, "VSMain");

	_rc->GaussianBlurX = std::make_unique<Shader>();
	_rc->GaussianBlurX->Initialize(L"./Engine/Assets/Shaders/GaussianBlur.hlsl",
	                               COMPILE_VERTEX | COMPILE_PIXEL | COMPILE_GEOM, USE_POSITION | USE_COLOR, "VS_Blur",
	                               "PS_BlurX", "GSMain");

	_rc->GaussianBlurY = std::make_unique<Shader>();
	_rc->GaussianBlurY->Initialize(L"./Engine/Assets/Shaders/GaussianBlur.hlsl",
	                               COMPILE_VERTEX | COMPILE_PIXEL | COMPILE_GEOM, USE_POSITION | USE_COLOR, "VS_Blur",
	                               "PS_BlurY", "GSMain");
}

void RenderInitSystem::CreateBlendStates() const
{
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	blendDesc.IndependentBlendEnable = false;

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable = true;

	rtbd.SrcBlend = D3D11_BLEND_ONE;
	rtbd.DestBlend = D3D11_BLEND_ONE;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ZERO;
	//!!!! ???
	rtbd.DestBlendAlpha = D3D11_BLEND_ONE;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	blendDesc.RenderTarget[0] = rtbd;

	HRESULT hr = _hc->device->CreateBlendState(&blendDesc, _rc->LightBlendState.GetAddressOf());


	D3D11_BLEND_DESC blendDesc2;
	ZeroMemory(&blendDesc2, sizeof(blendDesc2));

	blendDesc2.IndependentBlendEnable = false;
	blendDesc2.AlphaToCoverageEnable = false;

	D3D11_RENDER_TARGET_BLEND_DESC rtbd2;
	ZeroMemory(&rtbd2, sizeof(rtbd2));

	rtbd2.BlendEnable = true;

	rtbd2.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	rtbd2.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd2.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd2.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd2.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd2.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd2.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	blendDesc2.RenderTarget[0] = rtbd2;

	hr = _hc->device->CreateBlendState(&blendDesc2, _rc->ParticlesBlendState.GetAddressOf());


	D3D11_BLEND_DESC blendDescEditorGrid;
	ZeroMemory(&blendDescEditorGrid, sizeof(blendDescEditorGrid));

	blendDescEditorGrid.IndependentBlendEnable = false;

	D3D11_RENDER_TARGET_BLEND_DESC rtbdEditorGrid;
	ZeroMemory(&rtbdEditorGrid, sizeof(rtbdEditorGrid));

	rtbdEditorGrid.BlendEnable = true;

	rtbdEditorGrid.SrcBlend = D3D11_BLEND_ONE;
	rtbdEditorGrid.DestBlend = D3D11_BLEND_ONE;
	rtbdEditorGrid.BlendOp = D3D11_BLEND_OP_ADD;
	rtbdEditorGrid.SrcBlendAlpha = D3D11_BLEND_ZERO;
	//!!!! ???
	rtbdEditorGrid.DestBlendAlpha = D3D11_BLEND_ONE;
	rtbdEditorGrid.BlendOpAlpha = D3D11_BLEND_OP_MAX;
	rtbdEditorGrid.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	blendDescEditorGrid.RenderTarget[0] = rtbdEditorGrid;

	hr = _hc->device->CreateBlendState(&blendDescEditorGrid, _rc->GridBlendState.GetAddressOf());
}

void RenderInitSystem::CreateBuffers() const
{
	_rc->OpaqueConstBuffer = std::make_unique<Buffer>(_hc->device.Get());
	_rc->OpaqueConstBuffer->Initialize(sizeof(CB_BaseEditorBuffer));

	_rc->LightConstBuffer = std::make_unique<Buffer>(_hc->device.Get());
	_rc->LightConstBuffer->Initialize(sizeof(CB_LightBuffer));

	_rc->ShadowConstBuffer = std::make_unique<Buffer>(_hc->device.Get());
	_rc->ShadowConstBuffer->Initialize(sizeof(CB_ShadowBuffer));

	_rc->ShadowPointlightConstBuffer = std::make_unique<Buffer>(_hc->device.Get());
	_rc->ShadowPointlightConstBuffer->Initialize(sizeof(CB_PointlightShadowBuffer));

	_rc->GBuffer = std::make_unique<GBuffer>(_hc->device);
	_rc->GBuffer->Initialize(_hc->window->GetWidth(), _hc->window->GetHeight());
}

void RenderInitSystem::CreateSamplers() const
{
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT hr = _hc->device->CreateSamplerState(&sampDesc, _rc->SamplerState.GetAddressOf()); //Create sampler state


	D3D11_SAMPLER_DESC sampDesc2;
	ZeroMemory(&sampDesc, sizeof(sampDesc2));
	sampDesc2.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc2.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc2.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc2.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc2.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc2.MinLOD = 0;
	sampDesc2.MaxLOD = D3D11_FLOAT32_MAX;
	sampDesc2.MaxAnisotropy = 0;
	sampDesc2.MipLODBias = 0;
	hr = _hc->device->CreateSamplerState(&sampDesc2, _rc->ShadowMapSampler.GetAddressOf()); //Create sampler state


	sampDesc2.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	hr = _hc->device->CreateSamplerState(&sampDesc2, _rc->PointSampler.GetAddressOf()); //Create sampler state

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	// Create the texture sampler state.
	auto res2 = _hc->device->CreateSamplerState(&samplerDesc, _rc->SamplerDepthState.GetAddressOf());
}

void RenderInitSystem::CreateDepthAndStencils() const
{
	D3D11_DEPTH_STENCIL_DESC depthstencildesc;
	ZeroMemory(&depthstencildesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthstencildesc.DepthEnable = true;
	depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;
	depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;

	HRESULT hr = _hc->device->CreateDepthStencilState(&depthstencildesc, _rc->OffStencilState.GetAddressOf());

	depthstencildesc.DepthEnable = true;
	depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;
	depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER_EQUAL; //yup

	depthstencildesc.StencilEnable = true;
	depthstencildesc.StencilReadMask = 0xFF;
	depthstencildesc.StencilWriteMask = 0xFF;
	depthstencildesc.FrontFace.StencilFunc = D3D11_COMPARISON_NEVER;
	depthstencildesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthstencildesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_REPLACE;
	depthstencildesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	depthstencildesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthstencildesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	depthstencildesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthstencildesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	hr = _hc->device->CreateDepthStencilState(&depthstencildesc, _rc->BackFaceStencilState.GetAddressOf());

	depthstencildesc.DepthEnable = true;
	depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;

	depthstencildesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthstencildesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	depthstencildesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthstencildesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthstencildesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;

	depthstencildesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
	depthstencildesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;
	depthstencildesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthstencildesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	hr = _hc->device->CreateDepthStencilState(&depthstencildesc, _rc->FrontFaceStencilState.GetAddressOf());


	depthstencildesc.DepthEnable = true;
	depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;

	depthstencildesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depthstencildesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
	depthstencildesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthstencildesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthstencildesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	depthstencildesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
	depthstencildesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthstencildesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthstencildesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	hr = _hc->device->CreateDepthStencilState(&depthstencildesc, _rc->FinalPassStencilState.GetAddressOf());


	D3D11_DEPTH_STENCIL_DESC depthstencildesc2;
	ZeroMemory(&depthstencildesc2, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthstencildesc2.DepthEnable = true;
	depthstencildesc2.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	depthstencildesc2.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

	hr = _hc->device->CreateDepthStencilState(&depthstencildesc2, _rc->ShadowStencilState.GetAddressOf());


	/*
	 * Creating Views for depthStencil
	 *
	 */

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	depthStencilViewDesc.Texture2DArray.MipSlice = 0;
	depthStencilViewDesc.Texture2DArray.FirstArraySlice = 0;
	depthStencilViewDesc.Texture2DArray.ArraySize = 4;

	hr = _hc->device->CreateDepthStencilView(_rc->Texture, &depthStencilViewDesc, &_rc->ShadowStencilView);
}

void RenderInitSystem::CreateShaderResourceViewsAndRenderTargets() const
{
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	shaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2DArray.MipLevels = 1;
	shaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
	shaderResourceViewDesc.Texture2DArray.ArraySize = 4;


	HRESULT hr = _hc->device->CreateShaderResourceView(_rc->Texture, &shaderResourceViewDesc, &_rc->ShadowResourceView);

	//Shadow maps -->


	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc_;
	ZeroMemory(&shaderResourceViewDesc_, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shaderResourceViewDesc_.Format = DXGI_FORMAT_R32_FLOAT;
	shaderResourceViewDesc_.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc_.Texture2D.MipLevels = 1;


	hr = _hc->device->CreateShaderResourceView(_rc->TexturePCF, &shaderResourceViewDesc_, &_rc->ShadowMapResourceView);

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	hr = _hc->device->CreateRenderTargetView(_rc->TexturePCF, &renderTargetViewDesc, &_rc->ShadowMapRTV);

	HRESULT result;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc2;


	// Setup the description of the render target view.

	renderTargetViewDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	renderTargetViewDesc.Texture2DArray.MipSlice = 0;
	renderTargetViewDesc.Texture2DArray.ArraySize = 4;
	renderTargetViewDesc.Texture2DArray.FirstArraySlice = 0;


	// Create the render target view.
	result = _hc->device->CreateRenderTargetView(_rc->MRenderTargetTexture, &renderTargetViewDesc,
	                                             &_rc->MRenderTargetView);
	result = _hc->device->CreateRenderTargetView(_rc->MBluredShadowTexture, &renderTargetViewDesc,
	                                             &_rc->MBluredShadowRtv);

	//result=engine->device->CreateRenderTargetView(MRenderTargetTexture, nullptr, &MRenderTargetView);
	// Setup the description of the shader resource view.
	shaderResourceViewDesc2.Format = DXGI_FORMAT_R32G32_FLOAT;
	shaderResourceViewDesc2.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	shaderResourceViewDesc2.Texture2DArray.MostDetailedMip = 0;
	shaderResourceViewDesc2.Texture2DArray.MipLevels = 1;
	shaderResourceViewDesc2.Texture2DArray.ArraySize = 4;
	shaderResourceViewDesc2.Texture2DArray.FirstArraySlice = 0;


	// Create the shader resource view.
	result = _hc->device->CreateShaderResourceView(_rc->MRenderTargetTexture, &shaderResourceViewDesc2,
	                                               &_rc->MShaderResourceView);
	result = _hc->device->CreateShaderResourceView(_rc->MBluredShadowTexture, &shaderResourceViewDesc2,
	                                               &_rc->MBluredShadowSrv);

	//Creating instance ID Texture

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	result = _hc->device->CreateShaderResourceView(_rc->EntityIdTexture.Get(), &srvDesc,
	                                               _rc->EntityIdSrv.GetAddressOf());
}

void RenderInitSystem::CreateRasterizerStates() const
{
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_FRONT;

	_hc->device->CreateRasterizerState(&desc, _rc->CullFrontRasterizerState.GetAddressOf());

	desc.CullMode = D3D11_CULL_BACK;
	_hc->device->CreateRasterizerState(&desc, _rc->CullBackRasterizerState.GetAddressOf());

	desc.CullMode = D3D11_CULL_NONE;
	_hc->device->CreateRasterizerState(&desc, _rc->CullNoneRasterizerState.GetAddressOf());
}
