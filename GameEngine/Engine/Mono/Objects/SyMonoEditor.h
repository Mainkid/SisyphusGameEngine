#pragma once
#include "../SyMonoMethod.h"
#include "../SyMonoObj.h"
#include "../SyMonoProxyDatas.h"

class ResourceService;

namespace mono
{
	class ISyMonoEditorCallbacks;

	class SyMonoEditor : public SyMonoObj
	{
		inline static const std::string NAMESPACE = "SyEngine.Editor";
		inline static const std::string CLASS_NAME = "SyProxyEditor";

		inline static const std::string GE_INDENT = "GeIndent";
		inline static const std::string GE_DRAW_COMP_HEADER = "GeDrawCompHeader";
		inline static const std::string GE_DRAW_TEXT = "GeDrawText";
		inline static const std::string GE_DRAW_INT_FIELD = "GeDrawIntField";
		inline static const std::string GE_DRAW_FLOAT_FIELD = "GeDrawFloatField";
		inline static const std::string GE_DRAW_BOOL_FIELD = "GeDrawBoolField";
		inline static const std::string GE_DRAW_VECTOR3_FIELD = "GeDrawVector3Field";
		inline static const std::string GE_DRAW_COLOR_FIELD = "GeDrawColorField";
		inline static const std::string GE_DRAW_ENUM_FIELD = "GeDrawEnumField";
		inline static const std::string GE_DRAW_RES_FIELD = "GeDrawResField";
		inline static const std::string GE_DRAW_ARRAY_HEAD = "GeDrawArrayHead";
		inline static const std::string GE_DRAW_ARRAY_ITEM_BUTTONS = "GeDrawArrayItemButtons";
		inline static const std::string GE_DRAW_ARRAY_ADD_BUTTON = "GeDrawArrayAddButton";
		inline static const std::string GE_DRAW_ADD_COMP_MENU = "GeDrawAddCompMenu";

	private:
		static void GeIndent(bool isIncrease);
		static int GeDrawCompHeader(MonoString* rawName);
		static void GeDrawText(MonoString* rawName);
		static int GeDrawIntField(MonoString* rawName, int val);
		static float GeDrawFloatField(MonoString* rawName, float val);
		static bool GeDrawBoolField(MonoString* rawName, bool val);
		static ProxyVector3 GeDrawVector3Field(MonoString* rawName, ProxyVector3 val);
		static ProxyVector4 GeDrawColorField(MonoString* rawName, ProxyVector4 val);
		static int GeDrawEnumField(MonoString* rawName, MonoArray* rawItems, int selected);
		static MonoString* GeDrawResField(MonoString* rawName, EProxyResourceType rawResType, MonoString* rawSelectedUuid);
		static bool GeDrawArrayHead(MonoString* rawName);
		static int GeDrawArrayItemButtons(int idx);
		static bool GeDrawArrayAddButton();
		static int GeDrawAddCompMenu(MonoArray* rawComponents);

	public:
		SyMonoMethod<MonoObject*> EgInit{ "EgInit" };
		SyMonoMethod<uint32_t> EgDrawEntityComps {"EgDrawEntityComps"};

	private:
		inline static SyMonoEditor* _instance = nullptr;
		ResourceService* _resService = nullptr;

		SyResult OnAfterCreate() override;
		SyResult OnBeforeDestroy() override;
		const std::string& GetMonoClassName() override;
		const std::string& GetNamespace() override;
		bool IsUserClass() override;
	};
}
