#pragma once
#include <string>
#include <boost/uuid/uuid.hpp>
#include <mono/jit/jit.h>

namespace mono
{
	class SyMonoStr
	{
	public:
		SyMonoStr(MonoString* raw);

		~SyMonoStr();

		const char* GetChar();

		std::string ToStr();

		boost::uuids::uuid ToUuid();

		operator const char* ();

	private:
		MonoString* _raw = nullptr;
		char* _buff = nullptr;
	};
}