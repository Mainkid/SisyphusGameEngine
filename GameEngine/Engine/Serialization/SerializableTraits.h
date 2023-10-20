#pragma once

#include <type_traits>
#include <string>

#include "../../../vendor/entt/entt.hpp"

namespace ser
{
	template<typename T, typename = int>
	struct HasSerializeFunc : std::false_type
	{
	};
	template<typename T>
	struct HasSerializeFunc<T, decltype(&T::Serialize, 0)> : std::true_type
	{
	};

	template<typename T, typename = int>
	struct HasSerializeOuterFunc : std::false_type
	{
	};
	template<typename T>
	struct HasSerializeOuterFunc<T, decltype(SerializableOuter(std::declval<T>()), 0)> : std::true_type
	{
	};

	template<typename T>
	struct IsNativeSerializable : std::false_type
	{
	};
	template<>
	struct IsNativeSerializable<int> : std::true_type
	{
	};
	template<>
	struct IsNativeSerializable<float> : std::true_type
	{
	};
	template<>
	struct IsNativeSerializable<double> : std::true_type
	{
	};
	template<>
	struct IsNativeSerializable<bool> : std::true_type
	{
	};
	template<>
	struct IsNativeSerializable<std::string> : std::true_type
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