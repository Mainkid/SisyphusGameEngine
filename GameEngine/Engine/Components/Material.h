#pragma once
#include "../Features/Resources/ResourceBase.h"
#include "string"
#include "SimpleMath.h"
#include "Texture.h"
#include <d3d11.h>
#include <wrl.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <memory>

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

	//----User vars----
	boost::uuids::uuid albedoTextureUUID = boost::uuids::nil_uuid();
	boost::uuids::uuid specularTextureUUID = boost::uuids::nil_uuid();
	boost::uuids::uuid roughnessTextureUUID = boost::uuids::nil_uuid();
	boost::uuids::uuid metallicTextureUUID = boost::uuids::nil_uuid();
	boost::uuids::uuid emissiveTextureUUID = boost::uuids::nil_uuid();
	boost::uuids::uuid normalmapTextureUUID = boost::uuids::nil_uuid();
	boost::uuids::uuid opacityTextureUUID = boost::uuids::nil_uuid();


	DirectX::SimpleMath::Vector4 albedoValue = DirectX::SimpleMath::Vector4(0, 0, 0, -1);
	DirectX::SimpleMath::Vector4 specularValue = DirectX::SimpleMath::Vector4(0, 0, 0, -1);
	DirectX::SimpleMath::Vector4 roughnessValue = DirectX::SimpleMath::Vector4(0, 0, 0, -1);
	DirectX::SimpleMath::Vector4 metallicValue = DirectX::SimpleMath::Vector4(0, 0, 0, -1);
	DirectX::SimpleMath::Vector4 emissiveValue = DirectX::SimpleMath::Vector4(0, 0, 0, -1);;


	EMaterialBlendMode blendMode = EMaterialBlendMode::Opaque;

	//----Engine vars----
	std::shared_ptr<Texture> albedoSRV;
	std::shared_ptr<Texture> specularSRV;
	std::shared_ptr<Texture> roughnessSRV;
	std::shared_ptr<Texture> metallicSRV;
	std::shared_ptr<Texture> emissiveSRV;
	std::shared_ptr<Texture> normalmapSRV;
	std::shared_ptr<Texture> opacitySRV;

	
	ID3D11ShaderResourceView* resources[7];
	uint32_t hash = 0;
};

//namespace std
//{
//	template<> struct hash<Material>
//	{
//		using argument_type = Material;
//		using result_type = std::size_t;
//		result_type operator()(argument_type const& a) const
//		{
//			result_type const h1(std::hash<std::string>()(a.albedoTextureUUID));
//			result_type const h2(std::hash<std::string>()(a.roughnessTextureUUID));
//			result_type const h3(std::hash<std::string>()(a.specularTextureUUID));
//			result_type const h4(std::hash<std::string>()(a.metallicTextureUUID));
//			result_type const h5(std::hash<std::string>()(a.emissiveTextureUUID));
//			result_type const h6(std::hash<std::string>()(a.normalmapTextureUUID));
//			result_type const h7(std::hash<std::string>()(a.opacityTextureUUID));
//			return h1 * 37 + (h1 * 37 + h2) * 37 +
//				((h1 * 37 + h2) * 37 + h3) * 37 +
//				(((h1 * 37 + h2) * 37 + h3) * 37 + h4) * 37 +
//				((((h1 * 37 + h2) * 37 + h3) * 37 + h4) * 37 + h5) * 37 +
//				(((((h1 * 37 + h2) * 37 + h3) * 37 + h4) * 37 + h5) * 37 + h6) * 37 +
//				((((((h1 * 37 + h2) * 37 + h3) * 37 + h4) * 37 + h5) * 37 + h6) * 37 + h7);
//		}
//	};
//}