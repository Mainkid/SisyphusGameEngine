#include "SyMonoEditor.h"

#include "imgui.h"
#include "../../Core/Tools/MathHelper.h"
#include "../../Core/ServiceLocator.h"
#include "../../Features/Resources/ResourceService.h"
#include "../SyMonoStr.h"

using namespace mono;


void SyMonoEditor::GeIndent(bool isIncrease)
{
	if (isIncrease)
		ImGui::Indent(10.0f);
	else
		ImGui::Unindent(10.0f);
}

void SyMonoEditor::GeDrawSeparator(MonoString* rawName)
{
	if (rawName == nullptr)
	{
		ImGui::Separator();
	}
	else
	{
		SyMonoStr name{ rawName };
		ImGui::SeparatorText(name);
	}
}

void SyMonoEditor::GeDrawText(MonoString* rawName)
{
	if (rawName == nullptr)
	{
		ImGui::Text("[NULL]");
	}
	else
	{
		SyMonoStr name{ rawName };
		ImGui::Text(name);
	}
}

int SyMonoEditor::GeDrawIntField(MonoString* rawName, int val)
{
	SyMonoStr name{ rawName };
	ImGui::DragInt(name, &val);
	return val;
}

float SyMonoEditor::GeDrawFloatField(MonoString* rawName, float val)
{
	SyMonoStr name{ rawName };
	ImGui::DragFloat(name, &val, 0.1f);
	return val;
}

bool SyMonoEditor::GeDrawBoolField(MonoString* rawName, bool val)
{
	SyMonoStr name{ rawName };
	ImGui::Checkbox(name, &val);
	return val;
}

ProxyVector3 SyMonoEditor::GeDrawVector3Field(MonoString* rawName, ProxyVector3 val)
{
	SyMonoStr name{ rawName };
	float rawVal[3]{ val.X, val.Y, val.Z };
	ImGui::DragFloat3(name, rawVal);
	return { rawVal[0], rawVal[1], rawVal[2] };
}

ProxyVector4 SyMonoEditor::GeDrawColorField(MonoString* rawName, ProxyVector4 val)
{
	SyMonoStr name{ rawName };
	float rawVal[4]{ val.X, val.Y, val.Z, val.W };
	ImGui::ColorEdit4(name, rawVal);
	return { rawVal[0], rawVal[1], rawVal[2], rawVal[3] };
}

int SyMonoEditor::GeDrawEnumField(MonoString* rawName, MonoArray* rawItems, int selected)
{
	int count = mono_array_length(rawItems);
	if (count == 0)
		return 0;
	SyMathHelper::Clamp(selected, 0, count - 1);

	SyMonoStr name{ rawName };

	MonoString* rawSelectedItem = mono_array_get(rawItems, MonoString*, selected);
	SyMonoStr selectedItem{ rawSelectedItem };

	if (ImGui::BeginCombo(name, selectedItem))
	{
		for (int i = 0; i < count; i++)
		{
			MonoString* rawItem = mono_array_get(rawItems, MonoString*, i);
			SyMonoStr item { rawName };

			bool isSelected = i == selected;
			if (ImGui::Selectable(item, isSelected))
				selected = i;
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	return selected;
}

MonoString* SyMonoEditor::GeDrawResField(MonoString* rawName, EProxyResourceType rawResType, MonoString* rawSelectedUuid)
{
	if (_instance == nullptr || _instance->_resService == nullptr)
		return rawSelectedUuid;
	auto resService = _instance->_resService;

	auto resType = EAssetType::ASSET_NONE;
	if (rawResType == EProxyResourceType::Model)
		resType = EAssetType::ASSET_MESH;
	else if (rawResType == EProxyResourceType::Material)
		resType = EAssetType::ASSET_MATERIAL;
	if (resType == EAssetType::ASSET_NONE)
	{
		SY_LOG_MONO(SY_LOGLEVEL_ERROR, "failed to convert resource type");
		return rawSelectedUuid;
	}

	SyMonoStr name{ rawName };
	SyMonoStr strSelectedUuid{ rawSelectedUuid };

	boost::uuids::uuid selectedUuid = strSelectedUuid.ToUuid();

	std::string selectedName = selectedUuid.is_nil()
		? "none"
		: resService->FindFilePathByUUID(selectedUuid, true);

	bool isSelectedUuidChanged = false;

	if (ImGui::BeginCombo(name, selectedName.c_str()))
	{
		auto uuids = resService->GetAllResourcesOfType(resType);

		for (const auto& uuid : uuids)
		{
			bool isSelected = uuid == selectedUuid;
			std::string selectableName = resService->FindFilePathByUUID(uuid, false);
			if (ImGui::Selectable(selectableName.c_str(), isSelected))
			{
				isSelectedUuidChanged = true;
				selectedUuid = uuid;
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	if (isSelectedUuidChanged)
	{
		auto newStrSelectedUuid = boost::lexical_cast<std::string>(selectedUuid);
		auto monoDomain = mono_object_get_domain(_instance->GetInstance());
		rawSelectedUuid = mono_string_new(monoDomain, newStrSelectedUuid.c_str());
	}
	return rawSelectedUuid;
}

bool SyMonoEditor::GeDrawArrayHead(MonoString* rawName)
{
	SyMonoStr name{ rawName };
	return ImGui::CollapsingHeader(name);
}

int SyMonoEditor::GeDrawArrayItemButtons(int idx)
{
	int action = 0;
	ImGui::PushID(idx);
	ImGui::SameLine();
	if (ImGui::Button("-"))
		action = 1;
	ImGui::SameLine();
	if (ImGui::Button("/\\"))
		action = 2;
	ImGui::SameLine();
	if (ImGui::Button("\\/"))
		action = 3;
	ImGui::PopID();
	return action;
}

bool SyMonoEditor::GeDrawArrayAddButton()
{
	return ImGui::Button("+");
}


SyResult SyMonoEditor::OnAfterCreate()
{
	_instance = this;

	SY_RESULT_CHECK(EgInit.Bind(this));
	SY_RESULT_CHECK(EgDrawEntityComps.Bind(this));

	BindCallback(GE_INDENT, &GeIndent);
	BindCallback(GE_DRAW_SEPARATOR, &GeDrawSeparator);
	BindCallback(GE_DRAW_TEXT, &GeDrawText);
	BindCallback(GE_DRAW_INT_FIELD, &GeDrawIntField);
	BindCallback(GE_DRAW_FLOAT_FIELD, &GeDrawFloatField);
	BindCallback(GE_DRAW_BOOL_FIELD, &GeDrawBoolField);
	BindCallback(GE_DRAW_VECTOR3_FIELD, &GeDrawVector3Field);
	BindCallback(GE_DRAW_COLOR_FIELD, &GeDrawColorField);
	BindCallback(GE_DRAW_ENUM_FIELD, &GeDrawEnumField);
	BindCallback(GE_DRAW_RES_FIELD, &GeDrawResField);
	BindCallback(GE_DRAW_ARRAY_HEAD, &GeDrawArrayHead);
	BindCallback(GE_DRAW_ARRAY_ITEM_BUTTONS, &GeDrawArrayItemButtons);
	BindCallback(GE_DRAW_ARRAY_ADD_BUTTON, &GeDrawArrayAddButton);

	_resService = ServiceLocator::instance()->Get<ResourceService>();

	return {};
}

SyResult SyMonoEditor::OnBeforeDestroy()
{
	EgInit.UnBind();
	EgDrawEntityComps.UnBind();

	_instance = nullptr;
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