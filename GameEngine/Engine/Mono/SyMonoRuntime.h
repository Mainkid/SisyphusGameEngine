#pragma once
#include <string>

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"

namespace mono
{
	class SyMonoRuntime
	{
		inline static const std::string RUNTIME_PATH = "vendor/mono/lib";
		inline static const std::string DLL_LAYER_PATH = "MonoLayer.dll";
		inline static const std::string DLL_USER_PATH = "TestGame.dll";

	public:
		void Init();
		void Destroy();

		void Reload();

		MonoDomain* GetDomain() const;

		MonoImage* GetLayerAssemblyImage() const;
		MonoImage* GetUserAssemblyImage() const;

	private:
		MonoDomain* _rootDomain = nullptr;
		MonoDomain* _appDomain = nullptr;
		MonoAssembly* _layerAssembly = nullptr;
		MonoImage* _layerAssemblyImage = nullptr;
		MonoAssembly* _userAssembly = nullptr;
		MonoImage* _userAssemblyImage = nullptr;

		MonoAssembly* LoadAssembly(const std::string& dllPath);
		char* ReadFileBytes(const std::string& filepath, uint32_t* outSize);
	};
}