#pragma once

#include "SimpleMath.h"
#include <d3d11.h>
#include "../../Mesh/Components/MeshComponent.h"
#include <memory>
#include <vector>
#include "../../../Serialization/Serializable.h"
#include "../../../Core/ECS/SystemBase.h"
#include <boost/uuid/uuid_generators.hpp>

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
 
	~FSoundComponent() {
		
	}; 

	
//----User vars----
	bool SoundType3D = false;
	float SoundVolume = 1.0f;
	bool IsSoundPlaying = false;
	bool LoopedSound = false;
	boost::uuids::uuid SoundUuid = boost::uuids::nil_uuid(); // = "Engine/Assets/Audio/LookinAtIt.ogg";
	
//----Engine vars----
	boost::uuids::uuid ComponentUuid = boost::uuids::random_generator()();

	ESoundState State= ESoundState::Disabled;
	//bool HasStarted = false;
	int ChanelID = -1;

	size_t MonoHash = 0; //used by mono sync systems

	//SER_COMP(FSoundComponent, texturePath);
};

