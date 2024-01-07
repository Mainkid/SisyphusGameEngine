#pragma once
#include <memory>
#include <vector>
#include <set>
#include "../../Components/Material.h"
#include "../../Core/Rendering/GBuffer.h"
#include "../../Core/Graphics/Shader.h"
#include "../../Core/IService.h"
#include "../Features/Mesh/Mesh.h"
#include "../../vendor/HBAO/GFSDK_SSAO.h"

struct RenderHelperData
{
	ID3D11RenderTargetView* NullRtv[20] =
	{
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
	};

	ID3D11ShaderResourceView* NullSrv[20] =
	{
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
	};

	UINT strides12[1] = { 12 };
	UINT strides16[1] = { 16 };
	UINT strides32[1] = { 32 };
	UINT strides48[1] = { 48 };
	UINT strides64[1] = { 64 };
	UINT strides80[1] = { 80 };
	UINT offsets0[1] = { 0 };

	float bgColor0000[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float bgColor0001[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
};


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
	std::unique_ptr<Shader> EditorColliderRenderer;
	std::unique_ptr<Shader> GaussianBlurX;
	std::unique_ptr<Shader> GaussianBlurY;
	std::unique_ptr<Shader> IrradianceMapGenerator;
	std::unique_ptr<Shader> EnvironmentPrefilter;
	std::unique_ptr<Shader> IblLookUpGenerator;
	

	/*
	 * Particle System Shader;
	 */

	std::unique_ptr<Shader> SimpleParticle;
	std::unique_ptr<Shader> ParticleUpdate;
	std::unique_ptr<Shader> ParticleEmitter;
	std::unique_ptr<Shader> BitonicSort;
	std::unique_ptr<Shader> MatrixTranspose;
	std::unique_ptr<Shader> MultiplyBy6Shader;

	/*
	 * NavMesh shader;
	 */
	std::unique_ptr<Shader> NavMeshShader;
	std::unique_ptr<Shader> LineShader;


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
	

	/*
	    PCF
	*/

	ID3D11ShaderResourceView* ShadowMapResourceView;
	ID3D11Texture2D* TexturePCF;
	ID3D11RenderTargetView* ShadowMapRTV;

	/*
	    SkyBox
	*/


	std::unique_ptr<Buffer> ShadowConstBuffer;
	std::unique_ptr<Buffer> ShadowPointlightConstBuffer;

	std::set<std::unique_ptr<Material>> MaterialSet;


	/*
	 *	Ambient Occlusion
	 */

	GFSDK_SSAO_Parameters HbaoParams;
	GFSDK_SSAO_InputData_D3D11 Input;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> HbaoTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> HbaoSrv;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> HbaoRtv;
	
	GFSDK_SSAO_CustomHeap CustomHeap;
	GFSDK_SSAO_Status status;
	GFSDK_SSAO_Context_D3D11* pAOContext;

	RenderHelperData RhData;
};
