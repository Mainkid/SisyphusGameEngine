#pragma once

#include "SimpleMath.h"
#include <d3d11.h>
#include "../Components/Mesh.h"
#include <memory>
#include <vector>
#include "../Serialization/Serializable.h"

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
 
	~FSoundComponent() {}; 

	
//----User vars----
	bool SoundType3D = false;
	float SoundVolume = 1.0f;
	bool IsSoundPlaying = false;
	bool LoopedSound = false;
	// количество повторов 
	// 0 - бесконевчно
	std::string SoundPath = ""; // = "Engine/Assets/Audio/LookinAtIt.ogg";
	
//----Engine vars----
	bool IsON = false;
	//bool HasStarted = false;
	int ChanelID = -1;
	
	//SER_COMP(FSoundComponent, texturePath);
};

