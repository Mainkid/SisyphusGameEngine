#pragma once

#include "SimpleMath.h"
#include <d3d11.h>
#include "../Components/Mesh.h"
#include <memory>
#include <vector>
#include "../Serialization/Serializable.h"


//enum class ESoundType
//{
//	Sound2D,
//	Sound3D
//};

  /*  
    1) ��������� ���� - _privateMember
    2) ��������� ���� � ������ PublicMember
    3) ����������� ���� � ���������� CONST_VAR
    4) ��������� ���������� - localVar
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
	//ESoundType SoundType = ESoundType::Sound3D;
	bool Sound3D = false;
	float Volume = 1.0f;
	bool IsPlay = false;
	bool IsLooping = false;

	std::string SoundPath = ""; // = "Engine/Assets/Audio/LookinAtIt.ogg";
		

//----Engine vars----
	bool IsPlaying = false;
// TODO S:or should I use int32_t?
	int ChanelID;
 
	
	//SER_COMP(FSoundComponent, texturePath);
};

