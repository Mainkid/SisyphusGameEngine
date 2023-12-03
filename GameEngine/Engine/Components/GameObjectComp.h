#pragma once
#include <string>

#include "../Serialization/Serializable.h"

struct GameObjectComp
{
	//----User vars----
	std::string Name = "GameObject";
	std::string Tag = "None";



	size_t MonoHash = 0; // read/write only by mono sync system


	SER_COMP(GameObjectComp,
		Name,
		Tag)
};

