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
	hud->UpdateSelectedEntityEvent.AddRaw(this, &ViewportWidget::UpdateSelectedEntity);
}

void ViewportWidget::Render()
{
	ImGui::Begin(windowID.c_str());

	Widget::Render();
	
	ImVec2 imgSize = ImGui::GetContentRegionAvail();

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::ImageButton((void*)EngineCore::instance()->renderTarget->GetImageSRV(), ImGui::GetContentRegionAvail());
	ImVec2 r = ImGui::GetItemRectSize();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		
		ImVec2 pos2 = ImGui::GetMousePos();
		ImVec2 pos = ImGui::GetCursorScreenPos();

		auto textureSize = EngineCore::instance()->renderPipeline->GetRtvResolution();


		pos = ImVec2(pos2.x- pos.x, pos2.y - pos.y+imgSize.y);
	    x = std::clamp((int)(pos.x * textureSize.x / imgSize.x),0, (int)textureSize.x-1);
		y = std::clamp((int)(pos.y * textureSize.y / imgSize.y),0, (int)textureSize.y-1);
		
		int pixelColorR = EngineCore::instance()->renderPipeline->GetPixelValue(x, y);
		if (pixelColorR != -1)
		{
			hud->UpdateSelectedEntityEvent.Broadcast(entt::entity(pixelColorR));
		}

	}

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

void ViewportWidget::UpdateSelectedEntity(entt::entity)
{
	
}
