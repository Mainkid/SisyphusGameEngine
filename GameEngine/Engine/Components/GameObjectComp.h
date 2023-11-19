#pragma once
#include <string>

#include "../Serialization/Serializable.h"

struct GameObjectComp
{
	//----User vars----
	std::string Name = "GameObject";
	std::string Tag = "None";


	SER_COMP(GameObjectComp,
		Name,
		Tag)
};

