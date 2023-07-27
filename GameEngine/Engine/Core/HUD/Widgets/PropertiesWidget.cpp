#include "PropertiesWidget.h"

PropertiesWidget::PropertiesWidget(Hud* _hud)
{
	this->hud = _hud;
	this->windowID = "Properties";
}

void PropertiesWidget::Render()
{
	ImGui::Begin(windowID.c_str());
	Widget::Render();
	ImGui::End();
}
