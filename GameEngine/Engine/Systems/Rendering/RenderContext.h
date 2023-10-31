#pragma once
#include <memory>
#include <vector>
#include <set>
#include "../../Components/Material.h"
#include "../../Core/Rendering/GBuffer.h"
#include "../../Core/Graphics/Shader.h"
#include "../../Core/IService.h"
#include "../../Components/Mesh.h"

struct RenderContext : public IService
{
	ID3D11RenderTargetView* Rtvs[7];
	ID3D11RenderTargetView* EditorBillboardRtvs[2];

	std::unique_ptr<Shader> OpaqueShader;
	std::unique_ptr<Shader> DirLightShader;
	std::unique_ptr<Shader> AmbientLightShader;
	std::unique_ptr<Shader> PointLightShader;
	std::unique_ptr<Shader> StencilPassShader;
	std::unique_ptr<Shader> SpotLightShader;
	std::unique_ptr<Shader> ShadowShader;
	std::unique_ptr<Shader> ShadowPointLightShader;
	std::unique_ptr<Shader> BillboardShader;
	std::unique_ptr<Shader> ShadowMapGenerator;
	std::unique_ptr<Shader> ShadowMapPointLightGenerator;
	std::unique_ptr<Shader> SkyBoxShader;
	std::unique_ptr<Shader> ToneMapper;
	std::unique_ptr<Shader> EditorGridRenderer;
	std::unique_ptr<Shader> GaussianBlurX;
	std::unique_ptr<Shader> GaussianBlurY;


	std::shared_ptr<Mesh> CubeMesh;

	std::unique_ptr<GBuffer> GBuffer;
	Microsoft::WRL::ComPtr<ID3D11BlendState> LightBlendState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> ParticlesBlendState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> GridBlendState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> SamplerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> SamplerDepthState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> PointSampler;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> OffStencilState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> BackFaceStencilState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> FrontFaceStencilState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> FinalPassStencilState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> WriteOnlyStencilState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> CullFrontRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> CullBackRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> CullNoneRasterizerState;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> EntityIdTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> EntityIdSrv;


	D3D11_VIEWPORT Viewport = {};

	bool Helper = false;

	/*
	 * Screen Space Quad
	 *
	 */


	std::unique_ptr<Buffer> IndexQuadBuffer;
	std::unique_ptr<Buffer> VertexQuadBuffer;
	std::unique_ptr<Buffer> OpaqueConstBuffer;
	std::unique_ptr<Buffer> LightConstBuffer;

	/*
	 * Light AABB
	 *
	 */

	std::shared_ptr<Buffer> IndexLightAabbBuffer;
	std::shared_ptr<Buffer> VertexLightAabbBuffer;

	/*
	 * Shadows
	 *
	 */
	int ShadowmapWidth = 1024;
	int ShadowmapHeight = 1024;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> ShadowStencilState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> ShadowStencilView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> ShadowPointLightStencilView;
	ID3D11ShaderResourceView* ShadowResourceView;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> ShadowMapSampler;

	ID3D11Texture2D* Texture;
	ID3D11Texture2D* MRenderTargetTexture;
	ID3D11Texture2D* MBluredShadowTexture;
	ID3D11RenderTargetView* MRenderTargetView;
	ID3D11ShaderResourceView* MShaderResourceView;
	ID3D11ShaderResourceView* MBluredShadowSrv;
	ID3D11RenderTargetView* MBluredShadowRtv;

	/*
	    PCF
	*/

	ID3D11ShaderResourceView* ShadowMapResourceView;
	ID3D11Texture2D* TexturePCF;
	ID3D11RenderTargetView* ShadowMapRTV;

	/*
	    SkyBox
	*/

	Microsoft::WRL::ComPtr<ID3D11Texture2D> SkyboxTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SkyboxSRV;

	std::unique_ptr<Buffer> ShadowConstBuffer;
	std::unique_ptr<Buffer> ShadowPointlightConstBuffer;

	std::set<std::unique_ptr<Material>> MaterialSet;
};
