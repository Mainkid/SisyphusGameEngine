#pragma once
#include "IService.h"
#include <typeindex>
#include <memory>
#include <unordered_map>



class ServiceLocator
{
private:
	
public:
	std::unordered_map<std::type_index, std::shared_ptr<IService>> services;

	template <typename T>
	void Register()
	{
		
		if (services.find(typeid(T))!= services.end())
			return;
		
		services[typeid(T)] = std::make_shared<T>();
	}

	template <typename T>
	void Unregister()
	{
		if (services.find(typeid(T)) == services.end())
			return;
		services.erase(services.find(typeid(T)));
	}

	template <typename T>
	T* Get()
	{
		return (static_cast<T*>(services[typeid(T)].get()));
	}

	static ServiceLocator* instance()
	{
		static ServiceLocator inst;
		return &inst;
	}

};

