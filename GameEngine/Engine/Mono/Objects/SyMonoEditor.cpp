#include "SyMonoEditor.h"

#include "ImCurve.h"
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

int SyMonoEditor::GeDrawCompHeader(MonoString* rawName, bool isRemovable)
{
	int action = -1;

	SyMonoStr name{ rawName };
	ImGui::PushID(name);
	ImGui::SeparatorText(name);
	if (isRemovable && ImGui::Button("Remove"))
		action = 0;
	ImGui::PopID();
	return action;
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

bool SyMonoEditor::GeDrawIntField(MonoString* rawName, int* val)
{
	SyMonoStr name{ rawName };
	return ImGui::DragInt(name, val);
}

bool SyMonoEditor::GeDrawFloatField(MonoString* rawName, float* val)
{
	SyMonoStr name{ rawName };
	return ImGui::DragFloat(name, val, 0.1f);
}

bool SyMonoEditor::GeDrawBoolField(MonoString* rawName, bool* val)
{
	SyMonoStr name{ rawName };
	return ImGui::Checkbox(name, val);
}

bool SyMonoEditor::GeDrawVector2Field(MonoString* rawName, ProxyVector2* val)
{
	SyMonoStr name {rawName};
	float rawVal[2] {val->X, val->Y};
	bool isChanged = ImGui::DragFloat2(name, rawVal);
	val->X = rawVal[0];
	val->Y = rawVal[1];
	return isChanged;
}

bool SyMonoEditor::GeDrawVector3Field(MonoString* rawName, ProxyVector3* val)
{
	SyMonoStr name{ rawName };
	float rawVal[3]{ val->X, val->Y, val->Z };
	bool isChanged = ImGui::DragFloat3(name, rawVal);
	val->X = rawVal[0];
	val->Y = rawVal[1];
	val->Z = rawVal[2];
	return isChanged;
}

bool SyMonoEditor::GeDrawVector4Field(MonoString* rawName, ProxyVector4* val)
{
	SyMonoStr name {rawName};
	float rawVal[4] {val->X, val->Y, val->Z, val->W};
	bool isChanged = ImGui::DragFloat4(name, rawVal);
	val->X = rawVal[0];
	val->Y = rawVal[1];
	val->Z = rawVal[2];
	val->W = rawVal[3];
	return isChanged;
}


bool SyMonoEditor::GeDrawCurveField(MonoString* rawName, ProxyCurve* curve)
{
	SyMonoStr name{ rawName };

	bool isChanged = false;

	ImGui::Begin(name);
	ImGui::Text(name);
	ImGui::SameLine();

	bool isUsing = curve->IsUsing.X;
	isChanged |= ImGui::Checkbox("Is Using", &isUsing);
	float isUsingRaw = isUsing;
	curve->IsUsing = ProxyVector4{ isUsingRaw, isUsingRaw, isUsingRaw, isUsingRaw };

	if (!isUsing)
		return isChanged;

	bool isReversed = curve->IsReversed;
	isChanged |= ImGui::Checkbox("Is Reversed", &isReversed);

	curve->IsReversed = isReversed;
	curve->IsUsing.W = isReversed;

	float bezier[5] = { curve->P1.X, curve->P1.Y, curve->P2.X, curve->P2.Y, 0 };
	isChanged |= ImCurve::Bezier("Curve", bezier);

	curve->P1.X = bezier[0];
	curve->P1.Y = bezier[1];
	curve->P2.X = bezier[2];
	curve->P2.Y = bezier[3];

	return isChanged;
}


bool SyMonoEditor::GeDrawColorField(MonoString* rawName, ProxyVector4* val)
{
	SyMonoStr name{ rawName };
	float rawVal[4]{ val->X, val->Y, val->Z, val->W };
	bool isChanged = ImGui::ColorEdit4(name, rawVal);
	val->X = rawVal[0];
	val->Y = rawVal[1];
	val->Z = rawVal[2];
	val->W = rawVal[3];
	return isChanged;
}

bool SyMonoEditor::GeDrawEnumField(MonoString* rawName, MonoArray* rawItems, int* selected)
{
	int count = mono_array_length(rawItems);
	if (count == 0)
		return false;

	int prevSelected = *selected;

	SyMathHelper::Clamp(*selected, 0, count - 1);

	SyMonoStr name{ rawName };

	MonoString* rawSelectedItem = mono_array_get(rawItems, MonoString*, *selected);
	SyMonoStr selectedItem{ rawSelectedItem };

	if (ImGui::BeginCombo(name, selectedItem))
	{
		for (int i = 0; i < count; i++)
		{
			MonoString* rawItem = mono_array_get(rawItems, MonoString*, i);
			SyMonoStr item { rawItem };

			bool isSelected = i == *selected;
			if (ImGui::Selectable(item, isSelected))
				*selected = i;
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	return prevSelected != *selected;
}

bool SyMonoEditor::GeDrawResField(MonoString* rawName, EProxyResourceType rawResType, MonoString** rawSelectedUuid)
{
	if (_instance == nullptr || _instance->_resService == nullptr)
		return false;
	auto resService = _instance->_resService;

	auto resType = EAssetType::ASSET_NONE;
	if (rawResType == EProxyResourceType::Model)
		resType = EAssetType::ASSET_MESH;
	else if (rawResType == EProxyResourceType::Material)
		resType = EAssetType::ASSET_MATERIAL;
	else if (rawResType == EProxyResourceType::Cubemap)
		resType = EAssetType::ASSET_CUBEMAP;
	if (resType == EAssetType::ASSET_NONE)
	{
		SY_LOG_MONO(SY_LOGLEVEL_ERROR, "failed to convert resource type");
		return false;
	}

	SyMonoStr name{ rawName };
	SyMonoStr strSelectedUuid{ *rawSelectedUuid };

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
		*rawSelectedUuid = mono_string_new(monoDomain, newStrSelectedUuid.c_str());
	}
	return isSelectedUuidChanged;
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

bool SyMonoEditor::GeDrawFoldout(MonoString* rawName)
{
	SyMonoStr name{ rawName };
	return ImGui::CollapsingHeader(name);
}

int SyMonoEditor::GeDrawAddCompMenu(MonoArray* rawComponents)
{
	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("AddComponentPopup");

	int result = -1;

	if (ImGui::BeginPopupContextWindow("AddComponentPopup"))
	{

		int length = mono_array_length(rawComponents);
		for (int i = 0; i < length; i++)
		{
			MonoString* rawComp = mono_array_get(rawComponents, MonoString*, i);
			SyMonoStr comp{ rawComp };
			if (ImGui::Button(comp.GetChar()))
				result = i;
		}
		ImGui::EndPopup();
	}
	return result;
}


SyResult SyMonoEditor::OnAfterCreate()
{
	_instance = this;

	SY_RESULT_CHECK(EgInit.Bind(this));
	SY_RESULT_CHECK(EgDrawEntityComps.Bind(this));

	BindCallback(GE_INDENT, &GeIndent);
	BindCallback(GE_DRAW_COMP_HEADER, &GeDrawCompHeader);
	BindCallback(GE_DRAW_TEXT, &GeDrawText);
	BindCallback(GE_DRAW_INT_FIELD, &GeDrawIntField);
	BindCallback(GE_DRAW_FLOAT_FIELD, &GeDrawFloatField);
	BindCallback(GE_DRAW_BOOL_FIELD, &GeDrawBoolField);
	BindCallback(GE_DRAW_VECTOR2_FIELD, &GeDrawVector2Field);
	BindCallback(GE_DRAW_VECTOR3_FIELD, &GeDrawVector3Field);
	BindCallback(GE_DRAW_VECTOR4_FIELD, &GeDrawVector4Field);
	BindCallback(GE_DRAW_CURVE_FIELD, &GeDrawCurveField);
	BindCallback(GE_DRAW_COLOR_FIELD, &GeDrawColorField);
	BindCallback(GE_DRAW_ENUM_FIELD, &GeDrawEnumField);
	BindCallback(GE_DRAW_RES_FIELD, &GeDrawResField);
	BindCallback(GE_DRAW_ARRAY_HEAD, &GeDrawArrayHead);
	BindCallback(GE_DRAW_ARRAY_ITEM_BUTTONS, &GeDrawArrayItemButtons);
	BindCallback(GE_DRAW_ARRAY_ADD_BUTTON, &GeDrawArrayAddButton);
	BindCallback(GE_DRAW_FOLDOUT, &GeDrawFoldout);
	BindCallback(GE_DRAW_ADD_COMP_MENU, &GeDrawAddCompMenu);

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