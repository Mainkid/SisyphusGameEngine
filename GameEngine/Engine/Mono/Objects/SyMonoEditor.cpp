#include "SyMonoEditor.h"

#include "imgui.h"

using namespace mono;

void SyMonoEditor::GeDrawCompHeader(MonoString* rawName)
{
	char* name = mono_string_to_utf8(rawName);
	ImGui::Text(name);
}

int SyMonoEditor::GeDrawIntField(MonoString* rawName, int val)
{
	char* name = mono_string_to_utf8(rawName);
	ImGui::InputInt(name, &val);
	return val;
}

float SyMonoEditor::GeDrawFloatField(MonoString* rawName, float val)
{
	char* name = mono_string_to_utf8(rawName);
	ImGui::InputFloat(name, &val);
	return val;
}

bool SyMonoEditor::GeDrawBoolField(MonoString* rawName, bool val)
{
	char* name = mono_string_to_utf8(rawName);
	ImGui::Checkbox(name, &val);
	return val;
}

SyResult SyMonoEditor::OnAfterCreate()
{
	SY_RESULT_CHECK(EgInit.Bind(this));
	SY_RESULT_CHECK(EgDrawEntityCustomComps.Bind(this));

	BindCallback(GE_DRAW_COMP_HEADER, &GeDrawCompHeader);
	BindCallback(GE_DRAW_INT_FIELD, &GeDrawIntField);
	BindCallback(GE_DRAW_FLOAT_FIELD, &GeDrawFloatField);
	BindCallback(GE_DRAW_BOOL_FIELD, &GeDrawBoolField);

	return {};
}

SyResult SyMonoEditor::OnBeforeDestroy()
{
	EgInit.UnBind();
	EgDrawEntityCustomComps.UnBind();
	return {};
}

const std::string& SyMonoEditor::GetMonoClassName()
{
	return CLASS_NAME;
}

const std::string& SyMonoEditor::GetNamespace()
{
	return NAMESPACE;
}

bool SyMonoEditor::IsUserClass()
{
	return false;
}