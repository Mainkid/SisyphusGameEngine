#pragma once
#include <string>

#include "../Api/SyMonoObj.h"
#include "../Api/SyMonoMethod.h"

namespace mono
{
	class SyMonoLogger : public SyMonoObj
	{
		inline static const std::string NAMESPACE = "SyEngine.Logger";
		inline static const std::string CLASS_NAME = "SyProxyLogger";

		inline static const std::string GE_LOG = "GE_Log";

	private:
		static void GE_Log(int rawLevel, MonoString* rawTag, MonoString* rawMsg);

	private:
		SyResult OnAfterCreate() override;
		SyResult OnBeforeDestroy() override;
		const std::string& GetMonoClassName() override;
		const std::string& GetNamespace() override;
		bool IsUserClass() override;
	};
}
