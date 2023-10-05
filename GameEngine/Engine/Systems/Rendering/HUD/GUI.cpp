#include "GUI.h"


void GUI::DrawDropDown(const char* items[], const char* name, const int& numberOfElements)
{
	if (ImGui::TreeNode(name))
	{
		if (ImGui::BeginListBox(""))
		{
			static int item_current_idx = 0;
			for (int n = 0; n < numberOfElements; n++)
			{
				const bool is_selected = (item_current_idx == n);
				if (ImGui::Selectable(items[n], is_selected))
				{
					item_current_idx = n;
				}

				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();	// return int n ?
				}
			}
			ImGui::EndListBox();
		}
		ImGui::TreePop();
	}
}

void GUI::DrawInputField(float* var, const char* itemsName[], const int& numberOfElements)
{
	for (int i = 0; i < numberOfElements; i++)
	{
		ImGui::DragFloat(itemsName[i], &var[i], 0.1f);
	}
}

void GUI::DrawInputField(float* var, const char* name)
{
	ImGui::DragFloat(name, var, 0.1f);
}

void GUI::DrawCheckbox(bool* var, const char* itemsName[], const int& numberOfElements)
{
	for (int i = 0; i < numberOfElements; i++)
	{
		ImGui::Checkbox(itemsName[i], &var[i]);
	}
}

void GUI::DrawCheckbox(bool* var, const char* name)
{
	ImGui::Checkbox(name, var);
}

void GUI::DrawInputField(SyVector3& var, const char* name)
{
	float vec3[3]{ var.x, var.y, var.z };
	ImGui::DragFloat3(name, vec3, 0.1f);
	var.x = vec3[0];
	var.y = vec3[1];
	var.z = vec3[2];
}

void GUI::DrawInputField1(DirectX::SimpleMath::Vector4& var, const char* name)
{
	float vec4[4]{ var.x, var.y, var.z , var.w };
	ImGui::DragFloat4(name, vec4, 0.1f);
	var.x = vec4[0];
	var.y = vec4[1];
	var.z = vec4[2];
	var.w = vec4[3];
}

void GUI::DrawSlider(float& var, const char* name, const float& v_min, const float& v_max)
{
	ImGui::SliderFloat(name, &var, v_min, v_max);
};

void GUI::DrawColorPicker(DirectX::SimpleMath::Vector4& var, const char* name)
{
	float vec4[4]{ var.x, var.y, var.z , var.w };
	ImGui::ColorPicker4(name, vec4,1);
	var.x = vec4[0];
	var.y = vec4[1];
	var.z = vec4[2];
	var.w = vec4[3];
}
