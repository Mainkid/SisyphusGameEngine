#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include "../Widget.h"
#include "../Hud.h"

#include <map>
struct EngineContext;

class PropertiesWidget : public Widget
{
public:
	PropertiesWidget(Hud* _hud);
	void Render() override;
private:
	EngineContext* ec;
};

//const std::map<Com