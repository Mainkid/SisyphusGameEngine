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

enum class ESoundState
{
	Disabled,
	Playing,
	Paused
};

struct FSoundComponent
{

	FSoundComponent() 
	{ 
		
	};

	FSoundComponent(boost::uuids::uuid soundUuid)
	{
		this->SoundUuid = soundUuid;
	};
 
	~FSoundComponent() {}; 

	
//----User vars----
	bool SoundType3D = false;
	float SoundVolume = 1.0f;
	bool IsSoundPlaying = false;
	bool LoopedSound = false;
	boost::uuids::uuid SoundUuid = boost::uuids::nil_uuid(); // = "Engine/Assets/Audio/LookinAtIt.ogg";
	
//----Engine vars----
	ESoundState State= ESoundState::Disabled;
	//bool HasStarted = false;
	int ChanelID = -1;
	
	//SER_COMP(FSoundComponent, texturePath);
};

