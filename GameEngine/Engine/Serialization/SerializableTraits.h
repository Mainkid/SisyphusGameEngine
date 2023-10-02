#pragma once

#include <type_traits>
#include <string>

#include "../../../vendor/entt/entt.hpp"

namespace ser
{
	template<typename T, typename = int>
	struct HasSerializableFunc : std::false_type
	{
	};
	template<typename T>
	struct HasSerializableFunc<T, decltype(&T::Serialize, 0)> : std::true_type
	{
	};

	template<typename T>
	struct IsEntity : std::false_type
	{
	};
	template<>
	struct IsEntity<entt::entity> : std::true_type
	{
	};

	template<typename T, template<typename...> class Ref>
	struct IsCollection : std::false_type
	{
	};

	template<template<typename...> class Ref, typename... Args>
	struct IsCollection<Ref<Args...>, Ref> : std::true_type
	{
	};
}