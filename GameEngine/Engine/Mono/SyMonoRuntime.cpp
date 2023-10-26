#include "SyMonoRuntime.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <Windows.h>

using namespace mono;

void SyMonoRuntime::Init()
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
    {
        std::cout << "[mono_engine] failed to init lib" << std::endl;
    }
}

void SyMonoRuntime::Destroy()
{
    mono_jit_cleanup(_rootDomain);
    _rootDomain = nullptr;
    _appDomain = nullptr;
    _layerAssembly = nullptr;
}

void SyMonoRuntime::Reload()
{
    char appDomainName[] = "MonoLayerDomain";
    MonoDomain* domain = mono_domain_create_appdomain(appDomainName, nullptr);
    mono_domain_set(domain, true);

    _layerAssembly = LoadAssembly(DLL_LAYER_PATH);
    if (_layerAssembly == nullptr)
        return;
    _layerAssemblyImage = mono_assembly_get_image(_layerAssembly);

    _userAssembly = LoadAssembly(DLL_USER_PATH);
    if (_userAssembly == nullptr)
        return;
    _userAssemblyImage = mono_assembly_get_image(_userAssembly);

    if (_appDomain != nullptr)
    {
        mono_domain_unload(_appDomain);
    }
    _appDomain = domain;
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

MonoAssembly* SyMonoRuntime::LoadAssembly(const std::string& dllPath)
{
    uint32_t fileSize = 0;
    char* fileData = ReadFileBytes(dllPath, &fileSize);


    MonoImageOpenStatus status;
    MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

    if (status != MONO_IMAGE_OK)
    {
        const char* errorMessage = mono_image_strerror(status);
        std::cout << "[mono] assembly load error: " << errorMessage << std::endl;
        return nullptr;
    }

    MonoAssembly* assembly = mono_assembly_load_from_full(image, dllPath.c_str(), &status, 0);
    mono_image_close(image);

    delete[] fileData;

    if (assembly == nullptr)
        std::cout << "assembly " << dllPath << " is not loaded" << std::endl;

    return assembly;
}

char* SyMonoRuntime::ReadFileBytes(const std::string& filepath, uint32_t* outSize)
{
    std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

    if (!stream)
    {
        std::cout << "[mono] reading assembly file failed" << std::endl;
        return nullptr;
    }

    std::streampos end = stream.tellg();
    stream.seekg(0, std::ios::beg);
    uint32_t size = end - stream.tellg();

    if (size == 0)
    {
        std::cout << "[mono] assembly is empty" << std::endl;
        return nullptr;
    }

    auto buffer = new char[size];
    stream.read((char*)buffer, size);
    stream.close();

    *outSize = size;
    return buffer;
}
