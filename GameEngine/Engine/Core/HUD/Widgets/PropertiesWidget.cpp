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

    // надо удалять в деструкторе?
    LightComponent* lc = GetScene()->registry.try_get<LightComponent>(hud->selectedEntityID);  


    if (lc)
	{	
		//зачем?
		//auto degToRadians = [](float angle) {return angle * M_PI / 180.0f; };
		if (ImGui::TreeNode("Light"))
		{
			// Picker
			Vector4 vec4D = lc->color;
			float vec4[4] = { vec4D.x, vec4D.y, vec4D.z, vec4D.w };
			//рисуем 
			ImGui::ColorPicker4("Color", vec4);
			// соханяем изменения
			lc->color = Vector4(vec4[0], vec4[1], vec4[2], vec4[3]);
    
 
			// Attenuation Radius
			vec4D = lc->paramsRadiusAndAttenuation;
			float radius = vec4D.x;
			//рисуем 
			ImGui::DragScalar("Attenuation Radius", ImGuiDataType_Float, &radius, 1.f);
			// соханяем изменения
			lc->paramsRadiusAndAttenuation = (Vector4(radius, vec4D.y, vec4D.z, vec4D.w));

			ImGui::TreePop();
        }
	}

	ImGui::End();
}
