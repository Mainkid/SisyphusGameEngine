#include "PropertiesWidget.h"
#include "../../../../vendor/ImGui/imgui.h"
#include "../../../../vendor/ImGui/imgui_impl_dx11.h"
#include "../../../../vendor/ImGui/imgui_impl_win32.h"
#include "../../../../vendor/ImGuizmo/ImGuizmo.h"

PropertiesWidget::PropertiesWidget(Hud* _hud)
{
	this->hud = _hud;
	this->windowID = "Properties";
}

void PropertiesWidget::Render()
{
	ImGui::Begin(windowID.c_str());
	Widget::Render();

	TransformComponent* tc = GetScene()->registry.try_get<TransformComponent>(hud->selectedEntityID);

	if (tc)
	{
		auto degToRadians = [](float angle) {return angle * M_PI / 180.0f; };
		ImGui::Text("Transform");
		Vector3 vec3Dx = tc->position;
		float vec3[3]{ vec3Dx.x,vec3Dx.y,vec3Dx.z };
		ImGui::DragFloat3("Translation", vec3,0.1f);
		tc->position=(Vector3(vec3[0], vec3[1], vec3[2]));

		vec3Dx =TransformHelper::GetRotationDegrees(*tc);
		vec3[0] = vec3Dx.x;
		vec3[1] = vec3Dx.y;
		vec3[2] = vec3Dx.z;

		ImGui::DragFloat3("Rotation", vec3, 1.0f);
		auto vec = Vector3(vec3[0], vec3[1], vec3[2]);
		TransformHelper::DegreesToRad(vec);
		tc->rotation = vec;

		vec3Dx = tc->scale;
		vec3[0] = vec3Dx.x;
		vec3[1] = vec3Dx.y;
		vec3[2] = vec3Dx.z;
		ImGui::DragFloat3("Scale", vec3, 0.1f);
		tc->scale=(Vector3(vec3[0],vec3[1], vec3[2]));
	}

   
	//hud->selectedEntityID.


	ImGui::End();
}
