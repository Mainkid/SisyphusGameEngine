#pragma once
#include "string"
#include "SimpleMath.h"
#include <d3d11.h>
#include <wrl.h>

enum  class EMaterialBlendMode
{
	Opaque,
	Translucent
};

struct Material
{
	Material() {};
	Material(std::string albedo, std::string specular="", std::string roughness="", std::string metallic="", std::string emissive="", std::string normalMap="", std::string opacity="")
	{
		this->albedoTexturePath = albedo;
		this->specularTexturePath = specular;
		this->roughnessTexturePath = roughness;
		this->metallicTexturePath = metallic;
		this->emissiveTexturePath = emissive;
		this->normalmapTexturePath = normalMap;
		this->opacityTexturePath = opacity;
	}

	std::string albedoTexturePath = "./Engine/Assets/Textures/default_abledo.png";
	std::string specularTexturePath = "./Engine/Assets/Textures/black_texture.png";
	std::string roughnessTexturePath = "./Engine/Assets/Textures/white_texture.png";
	std::string metallicTexturePath = "./Engine/Assets/Textures/white_texture.png";
	std::string emissiveTexturePath = "./Engine/Assets/Textures/alpha-transparent.png";
	std::string normalmapTexturePath = "./Engine/Assets/Textures/default_normalmap.jpg";
	std::string opacityTexturePath = "";

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> albedoSRV;
	DirectX::SimpleMath::Vector4 albedoValue = DirectX::SimpleMath::Vector4(0,0,0,-1);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> specularSRV;
	DirectX::SimpleMath::Vector4 specularValue  = DirectX::SimpleMath::Vector4(0, 0, 0, -1);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughnessSRV;
	float roughnessValue = -1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metallicSRV;
	DirectX::SimpleMath::Vector4 metallicValue = DirectX::SimpleMath::Vector4(0, 0, 0, -1);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> emissiveSRV;
	DirectX::SimpleMath::Vector4 emissiveValue = DirectX::SimpleMath::Vector4(0, 0, 0, -1);;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalmapSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> opacitySRV;

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
			result_type const h1(std::hash<std::string>()(a.albedoTexturePath));
			result_type const h2(std::hash<std::string>()(a.roughnessTexturePath));
			result_type const h3(std::hash<std::string>()(a.specularTexturePath));
			result_type const h4(std::hash<std::string>()(a.metallicTexturePath));
			result_type const h5(std::hash<std::string>()(a.emissiveTexturePath));
			result_type const h6(std::hash<std::string>()(a.normalmapTexturePath));
			result_type const h7(std::hash<std::string>()(a.opacityTexturePath));
			return h1 * 37 + (h1 * 37 + h2) * 37 +
				((h1 * 37 + h2) * 37 + h3) * 37 +
				(((h1 * 37 + h2) * 37 + h3) * 37 + h4) * 37 +
				((((h1 * 37 + h2) * 37 + h3) * 37 + h4) * 37 + h5) * 37 +
				(((((h1 * 37 + h2) * 37 + h3) * 37 + h4) * 37 + h5) * 37 + h6) * 37 +
				((((((h1 * 37 + h2) * 37 + h3) * 37 + h4) * 37 + h5) * 37 + h6) * 37 + h7);
		}
	};
}