#pragma once
#include "../Widget.h"
#include "../Hud.h"
#include "../../../DirectXSDK/entt/entt.hpp"
#include <string>







class ViewportWidget: public Widget
{
public:
	ViewportWidget(Hud* _hud);
	void Render() override;
	void HandleResize();
	void GetInput() override;
	void UpdateSelectedEntity(entt::entity);

private:
	ImVec2 widgetSize;
	int x;
	int y;

};

