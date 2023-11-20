#pragma once

//#include "../Tools/Data/Vector.h"
//
//
//struct SystemSCTransform
//{
//	SyVector3 Origin = { 0.0f, 0.0f, 0.0f };
//	//SyVector3 Rotation = { 0.0f, 0.0f, 0.0f };
//};


//#include "DirectXHelpers.h"
#include "SimpleMath.h"
#include <d3d11.h>
#include "../Components/Mesh.h"
#include "../Core/Rendering/Lights/ELightType.h"
#include <memory>
#include <vector>
#include "../Serialization/Serializable.h"




enum class ESoundType
{
	Sound2D,
	Sound3D
};


struct FSoundComponent
{
	FSoundComponent()
	{
		SoundPath = "Engine/Assets/Audio/LookinAtIt.ogg";
	};

	//FSoundComponent(std::string _soundPath) 
	//{
	//	this->SoundPath = _soundPath;
	//};

	~FSoundComponent() {}; 


	//Vector4 A;
	//DirectX::SimpleMath::Vector3 Transform;
	std::string SoundPath = "Engine/Assets/Audio/LookinAtIt.ogg";
	//ESoundType SoundType;
	//float Volume;

	//SER_COMP(FSoundComponent, texturePath);

};

