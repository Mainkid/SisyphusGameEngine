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

	TransformComponent* tc = GetScene()->registry.try_get<TransformComponent>(hud->selectedEntityID);

	if (tc)
	{
		auto degToRadians = [](float angle) {return angle * M_PI / 180.0f; };
		ImGui::Text("Transform");
		Vector3 vec3Dx = tc->GetPosition();
		float vec3[3]{ vec3Dx.x,vec3Dx.y,vec3Dx.z };
		ImGui::DragFloat3("Translation", vec3,0.1f);
		tc->SetPosition(Vector3(vec3[0], vec3[1], vec3[2]));

		vec3Dx = tc->GetRotationDegrees();
		vec3[0] = vec3Dx.x;
		vec3[1] = vec3Dx.y;
		vec3[2] = vec3Dx.z;

		ImGui::DragFloat3("Rotation", vec3, 1.0f);
		tc->SetRotationDegrees(Vector3(vec3[0],vec3[1],vec3[2]));

		vec3Dx = tc->GetScale();
		vec3[0] = vec3Dx.x;
		vec3[1] = vec3Dx.y;
		vec3[2] = vec3Dx.z;
		ImGui::DragFloat3("Scale", vec3, 0.1f);
		tc->SetScale(Vector3(vec3[0],vec3[1], vec3[2]));
	}

   
	//hud->selectedEntityID.


	ImGui::End();
}
