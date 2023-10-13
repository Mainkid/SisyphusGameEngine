#pragma once
#include "imgui.h"

// для Vector4
#include "SimpleMath.h"
// для SyVector3
#include "../../../Tools/Data/Vector.h"



/* TODO: Add validation checks everywhere?
		 Should I move the logic to "GUI.inl" or delete it?
*/

class GUI
{
public:	
	inline static bool CreateNewChapter(const char* name, const bool& flagOpen)
	{
		ImGui::Separator();
		ImGui::SetNextItemOpen(flagOpen, ImGuiCond_Once);
		return ImGui::TreeNode(name);
	};
	inline static void EndChapter() { ImGui::TreePop(); };
	inline static void End() { ImGui::End(); };


	inline static void Separate(const char* text) 
	{ 
		text ? ImGui::SeparatorText(text) : ImGui::Separator();
	};

	static void HelpMarker(const char* text)
	{ 
		ImGui::SameLine();
		ImGui::TextDisabled("(?)");
		if (ImGui::BeginItemTooltip())
		{
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(text);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	};



//TODO:I didn't quite understand how to output what I chose ?
// return int n ?
// Just pass index of chosen element by reference
	static void DrawDropDown(const char* items[], const char* name, const int& numberOfElements);


	static void DrawCheckbox(bool* var, const char* name);
	static void DrawCheckbox(bool* var, const char* itemsName[], const int& numberOfElements);	


//TODO: add for Vector3 && Vector2 && Vector4 ?
// Add all types of vectors
	static void DrawInputField(float* var, const char* name);
	static void DrawInputField(float* var, const char* itemsName[], const int& numberOfElements);	
	static void DrawInputField(SyVector3& var, const char* name);
	static void DrawInputField1(DirectX::SimpleMath::Vector4& var, const char* name);


	static void DrawSlider(float& var, const char* name, const float& v_min, const float& v_max);
//TODO: need to?
// 	   Yes
	//static void DrawSlider(float& var, enum element)
	//{
	//	enum Element { Element_Fire, Element_Earth, Element_Air, Element_Water, Element_COUNT };

	//	static int elem = Element_Fire;
	//	const char* elems_names[Element_COUNT] = { "Fire", "Earth", "Air", "Water" };
	//	const char* elem_name = (elem >= 0 && elem < Element_COUNT) ? elems_names[elem] : "Unknown";
	//	ImGui::SliderInt("slider enum", &elem, 0, Element_COUNT - 1, elem_name); // Use ImGuiSliderFlags_NoInput flag to disable CTRL+Click here.

	//};
//TODO: need to?
// 	   Yes
	//static void DrawSlider(float* var, const char* itemsName[], const int& numberOfElements, const float& v_min, const float& v_max){	};

//TODO: which class transmits "Drag'n'drop" and what type of variable ?
	// Look how it's done in Properties Widget in fresh commits.
	static void DrawDragDropField()
	{
		/// ?
	};

	static void DrawColorPicker(DirectX::SimpleMath::Vector4& var, const char* name);
};


/* don't delete this:


* color text
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Name");	// Yellow
	ImGui::TextDisabled("Disabled"); // gray
	

* a line on the entire horizon
	ImGui::Separator();
	ImGui::SeparatorText("Text in line");

	

* ignores space
	ImGui::SameLine();

*/
