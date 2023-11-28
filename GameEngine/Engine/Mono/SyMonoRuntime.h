#pragma once
#include <string>

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"

#include "../Core/Tools/ErrorLogger.h"

namespace mono
{
	class SyMonoRuntime
	{
		inline static const std::string RUNTIME_PATH = "vendor/mono/lib";
		inline static const std::string DLL_LAYER_PATH = "MonoLayer.dll";
		inline static const std::string DLL_USER_PATH = "TestGame.dll";

	public:
		SyResult Init();
		void Destroy();

		SyResult Reload();

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

		SyResult LoadAssembly(const std::string& dllPath, MonoAssembly** outAssembly);
		SyResult ReadFileBytes(const std::string& filepath, char** outBytes, uint32_t* outBytesSize);
	};
}
