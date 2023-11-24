#pragma once
#include <type_traits>
#include "../ErrorLogger.h"
//aaa
#define DEFINE_BITWISE_OPERATORS(EnumType)										    \
inline unsigned operator& (EnumType lhs, EnumType rhs)				                \
{																				    \
	if (std::is_enum_v<EnumType> == false)                                          \
	{                                                                               \
		SY_LOG_CORE(SY_LOGLEVEL_ERROR, "%s is not enumeration type.", #EnumType);   \
		return 0;                                                                   \
	};                                                                              \
	return (static_cast<unsigned>(lhs) & static_cast<unsigned>(rhs));               \
};                                                                                  \
inline unsigned operator& (unsigned lhs, EnumType rhs)				                \
{																				    \
	if (std::is_enum_v<EnumType> == false)                                          \
	{                                                                               \
		SY_LOG_CORE(SY_LOGLEVEL_ERROR, "%s is not enumeration type.", #EnumType);   \
		return 0;                                                                   \
	};                                                                              \
	return (lhs & static_cast<unsigned>(rhs));                                      \
};                                                                                  \
inline unsigned operator| (EnumType lhs, EnumType rhs)				                \
{																				    \
	if (std::is_enum_v<EnumType> == false)                                          \
	{                                                                               \
		SY_LOG_CORE(SY_LOGLEVEL_ERROR, "%s is not enumeration type.", #EnumType);   \
		return 0;                                                                   \
	};                                                                              \
	return (static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));               \
};                                                                                  \
inline unsigned operator| (unsigned lhs, EnumType rhs)				                \
{																				    \
	if (std::is_enum_v<EnumType> == false)                                          \
	{                                                                               \
		SY_LOG_CORE(SY_LOGLEVEL_ERROR, "%s is not enumeration type.", #EnumType);   \
		return 0;                                                                   \
	};                                                                              \
	return (lhs | static_cast<unsigned>(rhs));                                      \
};                                                                                  \
inline unsigned operator~ (EnumType value)				                            \
{																				    \
	if (std::is_enum_v<EnumType> == false)                                          \
	{                                                                               \
		SY_LOG_CORE(SY_LOGLEVEL_ERROR, "%s is not enumeration type.", #EnumType);   \
		return 0;                                                                   \
	};                                                                              \
	return ~(static_cast<unsigned>(value));                                         \
};           
