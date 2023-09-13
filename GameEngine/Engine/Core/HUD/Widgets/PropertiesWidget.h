#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
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