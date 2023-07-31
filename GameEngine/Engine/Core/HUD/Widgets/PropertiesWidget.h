#pragma once
#include "../Widget.h"
#include "../Hud.h"

#include <map>


class PropertiesWidget : public Widget
{
public:
	PropertiesWidget(Hud* _hud);
	void Render() override;
	
};

//const std::map<Com