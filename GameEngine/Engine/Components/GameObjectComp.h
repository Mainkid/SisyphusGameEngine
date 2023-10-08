#pragma once
#include <string>

#include "../Serialization/Serializable.h"

struct GameObjectComp
{
	std::string Name = "GameObject";
	std::string Tag = "None";


	SER_COMP(GameObjectComp,
		Name,
		Tag)
};

