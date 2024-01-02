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
    1) приватные поля - _privateMember
    2) публичные поля и методы PublicMember
    3) константные поля и переменные CONST_VAR
    4) локальные переменные - localVar
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
	boost::uuids::uuid ComponentUuid = boost::uuids::random_generator()();
	
//----Engine vars----
	ESoundState State= ESoundState::Disabled;
	//bool HasStarted = false;
	int ChanelID = -1;
	
	//SER_COMP(FSoundComponent, texturePath);
};

