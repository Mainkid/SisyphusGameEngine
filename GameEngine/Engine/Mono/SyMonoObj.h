#pragma once

#include <string>

#include "mono/jit/jit.h"

#include "../Tools/ErrorLogger.h"

namespace mono
{
	class SyMonoRuntime;

	class SyMonoObj
	{
	public:
		virtual ~SyMonoObj();

		SyResult Create(const SyMonoRuntime& runtime);
		SyResult Destroy();

		bool IsValid() const;

		MonoClass* GetClass() const;
		MonoObject* GetInstance() const;

	protected:
		virtual const std::string& GetMonoClassName() = 0;
		virtual const std::string& GetNamespace() = 0;
		virtual bool IsUserClass() = 0;

		virtual SyResult OnAfterCreate() = 0;
		virtual SyResult OnBeforeDestroy() = 0;
	private:
		bool _isValid = false;
		MonoClass* _class = nullptr;
		uint32_t _gcHandle = 0;
	};
}
