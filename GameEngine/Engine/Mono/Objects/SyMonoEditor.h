#pragma once
#include "../Api/SyMonoMethod.h"
#include "../Api/SyMonoObj.h"
#include "../Ecs/SyMonoProxyDatas.h"

class ResourceService;

namespace mono
{
	class SyMonoEditor : public SyMonoObj
	{
		inline static const std::string NAMESPACE = "SyEngine.Editor";
		inline static const std::string CLASS_NAME = "SyProxyEditor";

		inline static const std::string GE_INDENT = "GeIndent";
		inline static const std::string GE_BEGIN_COMP = "GeBeginComp";
		inline static const std::string GE_END_COMP = "GeEndComp";
		inline static const std::string GE_BEGIN_GROUP = "GeBeginGroup";
		inline static const std::string GE_END_GROUP = "GeEndGroup";
		inline static const std::string GE_SAME_LINE = "GeSameLine";
		inline static const std::string GE_DRAW_TEXT = "GeDrawText";
		inline static const std::string GE_DRAW_INT_FIELD = "GeDrawIntField";
		inline static const std::string GE_DRAW_FLOAT_FIELD = "GeDrawFloatField";
		inline static const std::string GE_DRAW_BOOL_FIELD = "GeDrawBoolField";
		inline static const std::string GE_DRAW_VECTOR2_FIELD = "GeDrawVector2Field";
		inline static const std::string GE_DRAW_VECTOR3_FIELD = "GeDrawVector3Field";
		inline static const std::string GE_DRAW_VECTOR4_FIELD = "GeDrawVector4Field";
		inline static const std::string GE_DRAW_CURVE_FIELD = "GeDrawCurveField";
		inline static const std::string GE_DRAW_COLOR_FIELD = "GeDrawColorField";
		inline static const std::string GE_DRAW_ENUM_FIELD = "GeDrawEnumField";
		inline static const std::string GE_DRAW_RES_FIELD = "GeDrawResField";
		inline static const std::string GE_DRAW_ENTITY_FIELD = "GeDrawEntityField";
		inline static const std::string GE_DRAW_ARRAY_ITEM_BUTTONS = "GeDrawArrayItemButtons";
		inline static const std::string GE_DRAW_BUTTON = "GeDrawButton";
		inline static const std::string GE_DRAW_FOLDOUT = "GeDrawFoldout";
		inline static const std::string GE_DRAW_ADD_COMP_MENU = "GeDrawAddCompMenu";

	private:
		static void GeIndent(bool isIncrease);
		static int GeBeginComp(MonoString* rawName, bool isRemovable);
		static void GeEndComp();
		static void GeBeginGroup(MonoString* rawName);
		static void GeEndGroup();
		static void GeSameLine();
		static void GeDrawText(MonoString* rawName);
		static bool GeDrawIntField(MonoString* rawName, int* val);
		static bool GeDrawFloatField(MonoString* rawName, float* val, float valMin, float valMax, float valStep);
		static bool GeDrawBoolField(MonoString* rawName, bool* val);
		static bool GeDrawVector2Field(MonoString* rawName, ProxyVector2* val);
		static bool GeDrawVector3Field(MonoString* rawName, ProxyVector3* val);
		static bool GeDrawVector4Field(MonoString* rawName, ProxyVector4* val);
		static bool GeDrawCurveField(MonoString* rawName, ProxyCurve* curve);
		static bool GeDrawColorField(MonoString* rawName, bool withAlpha, ProxyVector4* val);
		static bool GeDrawEnumField(MonoString* rawName, MonoArray* rawItems, int* selected);
		static bool GeDrawEntityField(MonoString* rawName, bool* isValid, uint32_t* rawEnt);
		static bool GeDrawResField(MonoString* rawName, EProxyResourceType rawResType, MonoString** rawSelectedUuid);
		static int GeDrawArrayItemButtons(int idx);
		static bool GeDrawButton(MonoString* rawName);
		static bool GeDrawFoldout(MonoString* rawName);
		static int GeDrawAddCompMenu(MonoArray* rawComponents);

	public:
		SyMonoMethod<MonoObject*> EgInit{ "EgInit" };
		SyMonoMethod<uint32_t> EgDrawEntityComps {"EgDrawEntityComps"};

		void BindEcs(entt::registry* ecs);

	private:
		inline static SyMonoEditor* _instance = nullptr;

		ResourceService* _resService = nullptr;
		entt::registry* _ecs = nullptr;

		SyResult OnAfterCreate() override;
		SyResult OnBeforeDestroy() override;
		const std::string& GetMonoClassName() override;
		const std::string& GetNamespace() override;
		bool IsUserClass() override;
	};
}
