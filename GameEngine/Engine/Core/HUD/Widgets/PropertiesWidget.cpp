#include "PropertiesWidget.h"
#include "../../../../vendor/ImGui/imgui.h"
#include "../../../../vendor/ImGui/imgui_impl_dx11.h"
#include "../../../../vendor/ImGui/imgui_impl_win32.h"
#include "../../../../vendor/ImGuizmo/ImGuizmo.h"
#include "../../ServiceLocator.h"
#include "../../../Systems/EngineContext.h"
#include "../../../Core/ServiceLocator.h"

PropertiesWidget::PropertiesWidget(Hud* _hud)
{
	this->hud = _hud;
	this->windowID = "Properties";
	this->ec = ServiceLocator::instance()->Get<EngineContext>();
}

void PropertiesWidget::Render()
{
	ImGui::Begin(windowID.c_str());
	Widget::Render();

	TransformComponent* tc = ec->scene->registry.try_get<TransformComponent>(hud->selectedEntityID);

	if (tc)
	{
		auto degToRadians = [](float angle) {return angle * M_PI / 180.0f; };
		ImGui::Text("Transform");
		Vector3 vec3Dx = tc->localPosition;
		float vec3[3]{ vec3Dx.x, vec3Dx.y, vec3Dx.z };
		ImGui::DragFloat3("Translation", vec3,0.1f);
		tc->localPosition=(Vector3(vec3[0], vec3[1], vec3[2]));

		vec3Dx =TransformHelper::GetRotationDegrees(*tc);
		vec3[0] = vec3Dx.x;
		vec3[1] = vec3Dx.y;
		vec3[2] = vec3Dx.z;

		ImGui::DragFloat3("Rotation", vec3, 1.0f);
		auto vec = Vector3(vec3[0], vec3[1], vec3[2]);
		TransformHelper::DegreesToRad(vec);
		tc->localRotation = vec;

		vec3Dx = tc->localScale;
		vec3[0] = vec3Dx.x;
		vec3[1] = vec3Dx.y;
		vec3[2] = vec3Dx.z;
		ImGui::DragFloat3("Scale", vec3, 0.1f);
		tc->localScale=(Vector3(vec3[0],vec3[1], vec3[2]));
	}

	ImGui::End();
}
