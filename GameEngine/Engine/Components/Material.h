#pragma once
#include "ResourceBase.h"
#include "string"
#include "SimpleMath.h"
#include "Texture.h"
#include <d3d11.h>
#include <wrl.h>

enum  class EMaterialBlendMode
{
	Opaque,
	Translucent
};

struct Material : public ResourceBase
{
	Material() {};
	Material(std::string albedo, std::string specular="", std::string roughness="", std::string metallic="", std::string emissive="", std::string normalMap="", std::string opacity="")
	{
	}

	std::string albedoTextureUUID = "";
	std::string specularTextureUUID = "";
	std::string roughnessTextureUUID = "";
	std::string metallicTextureUUID = "";
	std::string emissiveTextureUUID = "";
	std::string normalmapTextureUUID = "";
	std::string opacityTextureUUID = "";

	Texture* albedoSRV;
	Texture* specularSRV;
	Texture* roughnessSRV;
	Texture* metallicSRV;
	Texture* emissiveSRV;
	Texture* normalmapSRV;
	Texture* opacitySRV;

	DirectX::SimpleMath::Vector4 albedoValue = DirectX::SimpleMath::Vector4(0,0,0,-1);
	DirectX::SimpleMath::Vector4 specularValue  = DirectX::SimpleMath::Vector4(0, 0, 0, -1);
	DirectX::SimpleMath::Vector4 roughnessValue = DirectX::SimpleMath::Vector4(0, 0, 0, -1);
	DirectX::SimpleMath::Vector4 metallicValue = DirectX::SimpleMath::Vector4(0, 0, 0, -1);
	DirectX::SimpleMath::Vector4 emissiveValue = DirectX::SimpleMath::Vector4(0, 0, 0, -1);;


	EMaterialBlendMode blendMode = EMaterialBlendMode::Opaque;
	
	ID3D11ShaderResourceView* resources[7];
	uint32_t hash = 0;
};

namespace std
{
	template<> struct hash<Material>
	{
		using argument_type = Material;
		using result_type = std::size_t;
		result_type operator()(argument_type const& a) const
		{
			result_type const h1(std::hash<std::string>()(a.albedoTextureUUID));
			result_type const h2(std::hash<std::string>()(a.roughnessTextureUUID));
			result_type const h3(std::hash<std::string>()(a.specularTextureUUID));
			result_type const h4(std::hash<std::string>()(a.metallicTextureUUID));
			result_type const h5(std::hash<std::string>()(a.emissiveTextureUUID));
			result_type const h6(std::hash<std::string>()(a.normalmapTextureUUID));
			result_type const h7(std::hash<std::string>()(a.opacityTextureUUID));
			return h1 * 37 + (h1 * 37 + h2) * 37 +
				((h1 * 37 + h2) * 37 + h3) * 37 +
				(((h1 * 37 + h2) * 37 + h3) * 37 + h4) * 37 +
				((((h1 * 37 + h2) * 37 + h3) * 37 + h4) * 37 + h5) * 37 +
				(((((h1 * 37 + h2) * 37 + h3) * 37 + h4) * 37 + h5) * 37 + h6) * 37 +
				((((((h1 * 37 + h2) * 37 + h3) * 37 + h4) * 37 + h5) * 37 + h6) * 37 + h7);
		}
	};
}