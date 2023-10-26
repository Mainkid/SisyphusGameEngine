#pragma once
#include "SyMonoObj.h"

namespace mono
{
	class SyMonoGameConfig : public SyMonoObj
	{
		inline static const std::string NAMESPACE;
		inline static const std::string CLASS_NAME = "SyGameConfig";

	private:
		void OnAfterCreate() override;
		void OnBeforeDestroy() override;
		const std::string& GetMonoClassName() override;
		const std::string& GetNamespace() override;
		bool IsUserClass() override;
	};
}