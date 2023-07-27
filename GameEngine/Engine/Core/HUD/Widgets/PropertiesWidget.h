#pragma once
#include "../Widget.h"
#include "../Hud.h"


class PropertiesWidget : public Widget
{
public:
	PropertiesWidget(Hud* _hud);
	void Render() override;
	
};

