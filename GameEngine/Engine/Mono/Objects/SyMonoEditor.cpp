#include "SyMonoEditor.h"

#include "imgui.h"
#include "../../Tools/MathHelper.h"
#include "../../Core/ServiceLocator.h"
#include "../../Systems/ResourceService.h"

using namespace mono;

void SyMonoEditor::GeDrawCompHeader(MonoString* rawName)
{
	char* name = mono_string_to_utf8(rawName);
	ImGui::Text(name);
}

int SyMonoEditor::GeDrawIntField(MonoString* rawName, int val)
{
	char* name = mono_string_to_utf8(rawName);
	ImGui::DragInt(name, &val);
	return val;
}

float SyMonoEditor::GeDrawFloatField(MonoString* rawName, float val)
{
	char* name = mono_string_to_utf8(rawName);
	ImGui::DragFloat(name, &val, 0.1f);
	return val;
}

bool SyMonoEditor::GeDrawBoolField(MonoString* rawName, bool val)
{
	char* name = mono_string_to_utf8(rawName);
	ImGui::Checkbox(name, &val);
	return val;
}

ProxyVector3 SyMonoEditor::GeDrawVector3Field(MonoString* rawName, ProxyVector3 val)
{
	char* name = mono_string_to_utf8(rawName);
	float rawVal[3]{ val.X, val.Y, val.Z };
	ImGui::DragFloat3(name, rawVal);
	return { rawVal[0], rawVal[1], rawVal[2] };
}

ProxyVector4 SyMonoEditor::GeDrawColorField(MonoString* rawName, ProxyVector4 val)
{
	char* name = mono_string_to_utf8(rawName);
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

	char* name = mono_string_to_utf8(rawName);

	MonoString* rawSelectedItem = mono_array_get(rawItems, MonoString*, selected);
	char* selectedItem = mono_string_to_utf8(rawSelectedItem);

	if (ImGui::BeginCombo(name, selectedItem))
	{
		for (int i = 0; i < count; i++)
		{
			MonoString* rawItem = mono_array_get(rawItems, MonoString*, i);
			char* item = mono_string_to_utf8(rawItem);

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
	if (rawResType == EProxyResourceType::Texture)
		resType = EAssetType::ASSET_TEXTURE;
	else if (rawResType == EProxyResourceType::Material)
		resType = EAssetType::ASSET_MATERIAL;
	if (resType == EAssetType::ASSET_NONE)
	{
		SY_LOG_MONO(SY_LOGLEVEL_ERROR, "failed to convert resource type");
		return rawSelectedUuid;
	}

	char* name = mono_string_to_utf8(rawName);

	char* strSelectedUuid = mono_string_length(rawSelectedUuid) > 0
		? mono_string_to_utf8(rawSelectedUuid)
		: nullptr;
	auto selectedUuid = strSelectedUuid == nullptr
		? boost::uuids::nil_uuid()
		: boost::lexical_cast<boost::uuids::uuid>(strSelectedUuid);
	std::string selectedName = selectedUuid.is_nil()
		? "none"
		: resService->FindFilePathByUUID(selectedUuid, true);

	bool isSelectedUuidChanged = false;

	if (ImGui::BeginCombo(name, selectedName.c_str()))
	{
		auto uuids = resService->GetAllResourcesOfType(resType);
		for (auto uuid : uuids)
		{
			bool isSelected = uuid == selectedUuid;
			std::string selectableName = resService->FindFilePathByUUID(uuid, true);
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
		const char* newStrSelectedUuid = boost::lexical_cast<std::string>(selectedUuid).c_str();
		auto monoDomain = mono_object_get_domain(_instance->GetInstance());
		rawSelectedUuid = mono_string_new(monoDomain, newStrSelectedUuid);
	}
	return rawSelectedUuid;
}


SyResult SyMonoEditor::OnAfterCreate()
{
	_instance = this;

	SY_RESULT_CHECK(EgInit.Bind(this));
	SY_RESULT_CHECK(EgDrawEntityComps.Bind(this));

	BindCallback(GE_DRAW_COMP_HEADER, &GeDrawCompHeader);
	BindCallback(GE_DRAW_INT_FIELD, &GeDrawIntField);
	BindCallback(GE_DRAW_FLOAT_FIELD, &GeDrawFloatField);
	BindCallback(GE_DRAW_BOOL_FIELD, &GeDrawBoolField);
	BindCallback(GE_DRAW_VECTOR3_FIELD, &GeDrawVector3Field);
	BindCallback(GE_DRAW_COLOR_FIELD, &GeDrawColorField);
	BindCallback(GE_DRAW_ENUM_FIELD, &GeDrawEnumField);
	BindCallback(GE_DRAW_RES_FIELD, &GeDrawResField);

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