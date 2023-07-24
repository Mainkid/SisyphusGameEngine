#pragma once
#include "../Widget.h"
#include "../Hud.h"
#include <string>







class ViewportWidget: public Widget
{
public:
	ViewportWidget(Hud* _hud);
	void Render() override;
	void HandleResize();
	void GetInput() override;

private:
	ImVec2 widgetSize;

};

