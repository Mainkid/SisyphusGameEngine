#pragma once
#include "../SyMonoMethod.h"
#include "../SyMonoObj.h"

namespace mono
{
	class SyMonoEditor : public SyMonoObj
	{
		inline static const std::string NAMESPACE = "SyEngine.Editor";
		inline static const std::string CLASS_NAME = "SyProxyEditor";

		inline static const std::string GE_DRAW_COMP_HEADER = "GeDrawCompHeader";
		inline static const std::string GE_DRAW_INT_FIELD = "GeDrawIntField";
		inline static const std::string GE_DRAW_FLOAT_FIELD = "GeDrawFloatField";
		inline static const std::string GE_DRAW_BOOL_FIELD = "GeDrawBoolField";

	private:
		static void GeDrawCompHeader(MonoString* rawName);
		static int GeDrawIntField(MonoString* rawName, int val);
		static float GeDrawFloatField(MonoString* rawName, float val);
		static bool GeDrawBoolField(MonoString* rawName, bool val);


	public:
		SyMonoMethod<MonoObject*> EgInit{ "EgInit" };
		SyMonoMethod<uint32_t> EgDrawEntityCustomComps {"EgDrawEntityCustomComps"};

	private:
		SyResult OnAfterCreate() override;
		SyResult OnBeforeDestroy() override;
		const std::string& GetMonoClassName() override;
		const std::string& GetNamespace() override;
		bool IsUserClass() override;
	};
}