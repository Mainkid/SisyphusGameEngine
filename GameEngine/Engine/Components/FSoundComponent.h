#pragma once

#include "SimpleMath.h"
#include <d3d11.h>
#include "../Components/Mesh.h"
#include <memory>
#include <vector>
#include "../Serialization/Serializable.h"

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
 
	~FSoundComponent() {}; 

	
//----User vars----
	bool SoundType3D = false;
	float SoundVolume = 1.0f;
	bool IsSoundPlaying = false;
	bool LoopedSound = false;
	// ���������� �������� 
	// 0 - �����������
	std::string SoundPath = ""; // = "Engine/Assets/Audio/LookinAtIt.ogg";
	
//----Engine vars----
	bool IsON = false;
	//bool HasStarted = false;
	int ChanelID = -1;
	
	//SER_COMP(FSoundComponent, texturePath);
};

