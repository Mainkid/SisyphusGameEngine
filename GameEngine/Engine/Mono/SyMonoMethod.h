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

		void Bind(SyMonoObj* target)
		{
			_target = target;

			_method = mono_class_get_method_from_name(
				target->GetClass(),
				_name.c_str(),
				sizeof...(TParams));
			if (_method == nullptr)
				std::cout << "[mono] failed to find method " << _name << std::endl;
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


				//
				//auto tuple = std::tie(params...);
				//
				//packedParams[0] = &std::get<0>(tuple);
				//if constexpr (paramsCount > 1)
				//	packedParams[1] = &std::get<1>(tuple);
				//if constexpr (paramsCount > 2)
				//	packedParams[2] = &std::get<2>(tuple);
				//if constexpr (paramsCount > 3)
				//	packedParams[3] = &std::get<3>(tuple);
				//if constexpr (paramsCount > 4)
				//	packedParams[4] = &std::get<4>(tuple);
				//if constexpr (paramsCount > 5)
				//	packedParams[5] = &std::get<5>(tuple);

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