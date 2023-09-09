#pragma once
#include <string>

struct DataComponent
{
	DataComponent() = default;
	DataComponent(std::string name, std::string tag = "None")
	{
		this->name = name;
		this->tag = tag;
	}
	std::string name = "GameObject";
	std::string tag = "None";
};

