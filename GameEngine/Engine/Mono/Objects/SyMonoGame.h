#pragma once
#include <string>

#include "../Api/SyMonoMethod.h"
#include "../Api/SyMonoObj.h"
#include "../Ecs/SyMonoProxyDatas.h"

namespace mono
{
	class SyMonoGame : public SyMonoObj
	{
	private:
		inline static const std::string NAMESPACE = "SyEngine.Game";
		inline static const std::string CLASS_NAME = "SyProxyGame";

	public:
		SyMonoMethod<MonoObject*, MonoObject*, MonoObject*> EgInit{ "EgInit" };

		SyMonoMethod<ProxyTimeData> EgLoopRun{ "EgLoopRun" };
		SyMonoMethod<> EgLoopDestroy{ "EgLoopDestroy" };

		SyMonoMethod<> EgSaveScene{ "EgSaveScene" };

	private:
		SyResult OnAfterCreate() override;
		SyResult OnBeforeDestroy() override;
		const std::string& GetMonoClassName() override;
		const std::string& GetNamespace() override;
		bool IsUserClass() override;
	};
}