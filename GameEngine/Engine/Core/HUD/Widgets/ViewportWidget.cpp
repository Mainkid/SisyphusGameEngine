#include "ViewportWidget.h"
#include "../../EngineCore.h"

void ViewportWidget::Render()
{
	ImGui::Begin("Viewport");
	ImGui::Image(EngineCore::backTex,)
	ImGui::End();
}
