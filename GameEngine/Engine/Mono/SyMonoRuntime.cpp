#include "SyMonoRuntime.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <Windows.h>

#include "../Core/Tools/ErrorLogger.h"

using namespace mono;

SyResult SyMonoRuntime::Init()
{
    wchar_t buffer[MAX_PATH];
    GetModuleFileName(nullptr, buffer, sizeof(buffer));
    auto path = std::filesystem::path(buffer).parent_path().append("vendor").append("mono").append("runtime");
    auto pathLib = std::filesystem::path(path).append("lib");
    auto pathEtc = std::filesystem::path(path).append("etc");

    //mono_set_assemblies_path(R"(..\vendor\mono\lib)");
    mono_set_dirs(pathLib.string().c_str(), pathEtc.string().c_str());

    _rootDomain = mono_jit_init("RootDomain");
    if (_rootDomain == nullptr)
	    return SyResult{ SY_RESCODE_ERROR, "failed to init lib" };
    return {};
}

void SyMonoRuntime::Destroy()
{
    if (_rootDomain != nullptr)
        mono_jit_cleanup(_rootDomain);
    _rootDomain = nullptr;
    _appDomain = nullptr;
    _layerAssembly = nullptr;
    _layerAssemblyImage = nullptr;
    _userAssembly = nullptr;
    _userAssemblyImage = nullptr;
}

SyResult SyMonoRuntime::Reload()
{
    char appDomainName[] = "MonoLayerDomain";
    MonoDomain* domain = mono_domain_create_appdomain(appDomainName, nullptr);
    mono_domain_set(domain, true);

    SyResult r = LoadAssembly(DLL_LAYER_PATH, &_layerAssembly);
    SY_RESULT_CHECK_EXT(r, "layer assembly");
    _layerAssemblyImage = mono_assembly_get_image(_layerAssembly);

    r = LoadAssembly(DLL_USER_PATH, &_userAssembly);
    SY_RESULT_CHECK_EXT(r, "user assembly");
    _userAssemblyImage = mono_assembly_get_image(_userAssembly);

    if (_appDomain != nullptr)
    {
        mono_domain_unload(_appDomain);
    }
    _appDomain = domain;
    return {};
}

MonoDomain* SyMonoRuntime::GetDomain() const
{
    return _appDomain;
}

MonoImage* SyMonoRuntime::GetLayerAssemblyImage() const
{
    return _layerAssemblyImage;
}

MonoImage* SyMonoRuntime::GetUserAssemblyImage() const
{
    return _userAssemblyImage;
}

SyResult SyMonoRuntime::LoadAssembly(const std::string& dllPath, MonoAssembly** outAssembly)
{
    uint32_t fileSize = 0;
    char* fileData = nullptr;

    SyResult r = ReadFileBytes(dllPath, &fileData, &fileSize);
    SY_RESULT_CHECK(r);

    MonoImageOpenStatus status;
    MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

    if (status != MONO_IMAGE_OK)
    {
        delete[] fileData;
        return SyResult
        {
            SY_RESCODE_ERROR,
            SyComplexString("assembly image error: %s", mono_image_strerror(status))
        };
    }

    MonoAssembly* assembly = mono_assembly_load_from_full(image, dllPath.c_str(), &status, 0);
    mono_image_close(image);

    delete[] fileData;

    if (assembly == nullptr)
        return SyResult{ SY_RESCODE_ERROR, SyComplexString("assembly failed to loaded") };

    *outAssembly = assembly;
    return {};
}

SyResult SyMonoRuntime::ReadFileBytes(const std::string& filepath, char** outBytes, uint32_t* outBytesSize)
{
    std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

    if (!stream)
	    return SyResult{SY_RESCODE_ERROR, "cannot access assembly file"};

    std::streampos end = stream.tellg();
    stream.seekg(0, std::ios::beg);
    uint32_t size = end - stream.tellg();

    if (size == 0)
	    return SyResult{ SY_RESCODE_ERROR, "assembly is empty" };

    auto buffer = new char[size];
    stream.read(buffer, size);
    stream.close();

    *outBytes = buffer;
    *outBytesSize = size;
    return {};
}
