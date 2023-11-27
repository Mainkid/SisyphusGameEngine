#pragma once

#include "SimpleMath.h"
#include <d3d11.h>
#include "../Components/Mesh.h"
#include <memory>
#include <vector>
#include "../Serialization/Serializable.h"

//#include "Components/FAudioEngine.h"


enum class ESoundType
{
	Sound2D,
	Sound3D
};

  /*  
    1) приватные поля - _privateMember
    2) публичные поля и методы PublicMember
    3) константные поля и переменные CONST_VAR
    4) локальные переменные - localVar
  */

struct FSoundComponent
{

	FSoundComponent() 
	{ 
		//this->SoundType = ESoundType::Sound3D;
		//this->Volume = 1.0f;
		//this->IsPlay = 0;
		//this->IsLooping = 0;
		//this->SoundPath = "Engine/Assets/Audio/LookinAtIt.ogg";
		//this->Transform = Vector3::Zero;
	};

	FSoundComponent(std::string soundPath)
	{
		this->SoundPath = soundPath;
	};

	//FSoundComponent(ESoundType type, float volume, bool isoplay, bool ispooping, std::string soundPath, Vector3 pos)
	//{
	//	this->SoundType = type;
	//	this->Volume = volume;
	//	this->IsPlay = isoplay;
	//	this->IsLooping = ispooping;
	//	this->SoundPath = soundPath;
	//	this->Transform = pos;
	//};
 
	~FSoundComponent() {}; 

	
//----User vars----
	ESoundType SoundType = ESoundType::Sound3D;
	float Volume = 1.0f;
	bool IsPlay = false;

	bool IsLooping = false;
	std::string SoundPath = ""; // = "Engine/Assets/Audio/LookinAtIt.ogg";
	DirectX::SimpleMath::Vector3 Transform = Vector3::Zero;
	
//----Engine vars----
	bool Playing = false;
	// void SoundEnd();	
//    FAudioEngine TESTAE;
//    TESTAE.Init();
//    TESTAE.LoadSound("Engine/Assets/Audio/LookinAtIt.ogg");
//    TESTAE.PlayMSound("Engine/Assets/Audio/LookinAtIt.ogg");
	
	//SER_COMP(FSoundComponent, texturePath);
};

