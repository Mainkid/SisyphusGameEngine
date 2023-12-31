#include "SyMonoStr.h"

#include <string>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <boost/lexical_cast.hpp>

using namespace mono;

SyMonoStr::SyMonoStr(MonoString* raw)
{
	_raw  = raw;
	_buff = nullptr;
}

SyMonoStr::~SyMonoStr()
{
	if (_buff != nullptr)
	{
		mono_free(_buff);
		_buff = nullptr;
	}
}

const char* SyMonoStr::GetChar()
{
	if (_buff == nullptr)
	{
		if (_raw == nullptr)
			return nullptr;
		_buff = mono_string_to_utf8(_raw);
	}
	return _buff;
}

std::string SyMonoStr::ToStr()
{
	return GetChar();
}

boost::uuids::uuid SyMonoStr::ToUuid()
{
	const char* buff = GetChar();
	if (buff == nullptr)
		return boost::uuids::nil_uuid();
	std::string str{ buff };
	return boost::lexical_cast<boost::uuids::uuid>(str);
}

SyMonoStr::operator const char*()
{
	return GetChar();
}