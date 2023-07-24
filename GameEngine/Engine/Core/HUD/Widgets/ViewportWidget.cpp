#include "../../EngineCore.h"
#include "../../../Core/Rendering/RenderTarget.h"

#include "../Hud.h"

#include "ViewportWidget.h"
//#define STB_IMAGE_IMPLEMENTATION
//#include "../../../DirectXSDK/stb_image.h"

ViewportWidget::ViewportWidget(Hud* _hud)
{
	this->hud = _hud;
	this->windowID = "Viewport";
}

void ViewportWidget::Render()
{
	ImGui::Begin(windowID.c_str());

	Widget::Render();
	ImGui::Text("pointer = %p", EngineCore::instance()->renderTarget.get()->GetImageSRV());
	ImGui::Image((void*)EngineCore::instance()->renderTarget->GetImageSRV(), ImGui::GetContentRegionAvail());

	HandleResize();
	
	ImGui::End();
}

void ViewportWidget::HandleResize()
{
	auto newWidgetSize = ImGui::GetWindowSize();
	if (widgetSize.x != newWidgetSize.x || widgetSize.y != newWidgetSize.y)
	{
		hud->ViewportResizedEvent.Broadcast(newWidgetSize.x * 1.0f / newWidgetSize.y);
		widgetSize = newWidgetSize;
	}

}

void ViewportWidget::GetInput()
{
	
}
