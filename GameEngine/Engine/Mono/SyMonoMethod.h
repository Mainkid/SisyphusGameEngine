#pragma once

#include <iostream>
#include <string>

#include "SyMonoObj.h"
#include "mono/jit/jit.h"

namespace mono
{
	class SyMonoObj;
	
	template<typename ...TParams>
	class SyMonoMethod
	{
	public:
		SyMonoMethod(const std::string& name)
		{
			_name = name;

			for (int i = 0; i < sizeof...(TParams); i++)
				_params[i] = nullptr;
		}

		SyResult Bind(SyMonoObj* target)
		{
			_target = target;

			_method = mono_class_get_method_from_name(
				target->GetClass(),
				_name.c_str(),
				sizeof...(TParams));
			if (_method == nullptr)
				return SyResult{ SY_RESCODE_ERROR, SyComplexString("failed to find method %s", _name.c_str())};
			return {};
		}

		void UnBind()
		{
			_method = nullptr;
		}

		MonoObject* Invoke(TParams... params)
		{
			if (_method == nullptr)
				return nullptr;
			if (!_target->IsValid())
				return nullptr;

			MonoObject* exc = nullptr;
			MonoObject* result = nullptr;

			constexpr size_t paramsCount = sizeof...(params);
			if constexpr (paramsCount == 0)
			{
				result = mono_runtime_invoke(_method, _target->GetInstance(), nullptr, &exc);
			}
			else
			{
				void* packedParams[paramsCount];
				FillParams(packedParams, 0, params...);
				result = mono_runtime_invoke(_method, _target->GetInstance(), packedParams, &exc);
			}
			return exc == nullptr ? result : nullptr;
		}

	private:
		std::string _name;

		void* _params[sizeof...(TParams) == 0 ? 1 : sizeof...(TParams)];


		SyMonoObj* _target = nullptr;
		MonoMethod* _method = nullptr;

		void FillParams(void** pack, int i) { }

		template<typename T>
		void FillParams(void** pack, int i, T& param)
		{
			pack[i] = &param;
		}

		void FillParams(void** pack, int i, MonoObject* param)
		{
			pack[i] = param;
		}

		template<typename T, typename ...TOthers>
		void FillParams(void** pack, int i, T& param, TOthers&... otherParams)
		{
			pack[i] = &param;
			FillParams(pack, i + 1, otherParams...);
		}

		template<typename ...TOthers>
		void FillParams(void** pack, int i, MonoObject* param, TOthers&... otherParams)
		{
			pack[i] = param;
			FillParams(pack, i + 1, otherParams...);
		}
	};
}