#pragma once

#include <string>

#include "mono/jit/jit.h"

namespace mono
{
	class SyMonoRuntime;

	class SyMonoObj
	{
	public:
		virtual ~SyMonoObj();

		void Create(const SyMonoRuntime& runtime);
		void Destroy();

		bool IsValid() const;

		MonoClass* GetClass() const;
		MonoObject* GetInstance() const;

	protected:
		MonoMethod* FindMethod(const std::string& methodName, int paramsCount);
		void* BindMethod(const std::string& methodName, int paramsCount);

		virtual const std::string& GetMonoClassName() = 0;
		virtual const std::string& GetNamespace() = 0;
		virtual bool IsUserClass() = 0;

		virtual void OnAfterCreate() = 0;
		virtual void OnBeforeDestroy() = 0;
	private:
		bool _isValid = false;
		MonoClass* _class = nullptr;
		uint32_t _gcHandle = 0;
	};
}
