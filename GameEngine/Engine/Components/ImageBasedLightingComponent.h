#pragma once
#include <d3d11.h>
#include <wrl.h>
#include "../Serialization/Serializable.h"

struct ImageBasedLightingComponent
{
	Microsoft::WRL::ComPtr<ID3D11SamplerState> IrradianceMapSampler;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> IrradianceCubeMapTex;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> IrradianceCubeMapSrv;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> IrradianceCubeMapRtv;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> IrradianceCubeMapUav;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> FilteredEnvironmentCubeMapTex;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> FilteredEnvironmentSrv;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> IblLookUpTexture;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> IblLookUpUav;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> IblLookUpSrv;

	std::vector<Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>> FilteredEnvironmentCubeMapUavs;
	bool IsInitialized = false;
	bool IsOn = false;
	SER_COMP(ImageBasedLightingComponent, IsOn);
};
