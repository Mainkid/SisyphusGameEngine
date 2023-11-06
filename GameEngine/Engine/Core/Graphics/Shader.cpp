#include "Shader.h"
#include "../../Systems/HardwareContext.h"
#include "../ServiceLocator.h"

void Shader::Initialize(LPCWSTR shaderPath, unsigned int compile_flags, unsigned int uniform_flags,
    LPCSTR v_entryPoint, LPCSTR p_entryPoint, LPCSTR g_entryPoint, LPCSTR c_entryPoint)
{
    hc = ServiceLocator::instance()->Get<HardwareContext>();

    HRESULT res;
    if ((compile_flags & COMPILE_PIXEL) == COMPILE_PIXEL)
    {
        res = D3DCompileFromFile(shaderPath, nullptr /*macros*/, D3D_COMPILE_STANDARD_FILE_INCLUDE /*include*/,
            p_entryPoint, "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
            0, pixelBC.GetAddressOf(), errorPixelCode.GetAddressOf());
        if (FAILED(res))
        {
            if (errorPixelCode != nullptr)
                OutputDebugStringA((char*)errorPixelCode->GetBufferPointer());
        }

        res = hc->device->CreatePixelShader(
            this->pixelBC->GetBufferPointer(),
            this->pixelBC->GetBufferSize(),
            nullptr, this->pixelShader.GetAddressOf());
    }

    if ((compile_flags &COMPILE_VERTEX)==COMPILE_VERTEX)
    {
        res = D3DCompileFromFile(shaderPath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
            v_entryPoint, "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
            vertexBC.GetAddressOf(), errorVertexCode.GetAddressOf());

        if (FAILED(res))
        {
            if (errorVertexCode != nullptr)
                OutputDebugStringA((char*)errorVertexCode->GetBufferPointer());
        }

        res = hc->device->CreateVertexShader(
            this->vertexBC->GetBufferPointer(),
            this->vertexBC->GetBufferSize(),
            nullptr, this->vertexShader.GetAddressOf());
    }

    if ((compile_flags & COMPILE_GEOM) == COMPILE_GEOM)
    {
        res = D3DCompileFromFile(shaderPath, nullptr /*macros*/, nullptr /*include*/,
            g_entryPoint, "gs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
            0, geomBC.GetAddressOf(), errorPixelCode.GetAddressOf());

        res = hc->device->CreateGeometryShader(
            this->geomBC->GetBufferPointer(),
            this->geomBC->GetBufferSize(),
            nullptr, this->geomShader.GetAddressOf());

    }

    if ((compile_flags & COMPILE_COMPUTE) == COMPILE_COMPUTE)
    {
         res = D3DCompileFromFile(shaderPath, nullptr /*macros*/, nullptr /*include*/,
            c_entryPoint, "cs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
            0, computeBC.GetAddressOf(), errorPixelCode.GetAddressOf());

         if (FAILED(res))
         {
             if (errorVertexCode != nullptr)
                 OutputDebugStringA((char*)errorPixelCode->GetBufferPointer());
            
         }

         res = hc->device->CreateComputeShader(
             this->computeBC->GetBufferPointer(),
             this->computeBC->GetBufferSize(),
             nullptr, this->computeShader.GetAddressOf());
    }

    //UNIFORM parameters

    D3D11_INPUT_ELEMENT_DESC inputElements [5];
    int ctr = 0;

    if ((uniform_flags & USE_POSITION) == USE_POSITION)
    {
        inputElements[ctr]=D3D11_INPUT_ELEMENT_DESC{
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            0,
            D3D11_INPUT_PER_VERTEX_DATA,
            0 };
        ctr++;
    }

    if ((uniform_flags & USE_COLOR) == USE_COLOR)
    {
        inputElements[ctr]=D3D11_INPUT_ELEMENT_DESC{
            "COLOR",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0 };
        ctr++;
    }

    if ((uniform_flags & USE_NORMAL) == USE_NORMAL)
    {
        inputElements[ctr]=D3D11_INPUT_ELEMENT_DESC{
            "NORMAL",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0 };
        ctr++;
    }

    if ((uniform_flags & USE_TANGENT_BITANGENT) == USE_TANGENT_BITANGENT)
    {
        inputElements[ctr] = D3D11_INPUT_ELEMENT_DESC{
            "TANGENT",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0 };
        ctr++;

        inputElements[ctr] = D3D11_INPUT_ELEMENT_DESC{
            "BITANGENT",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0 };
        ctr++;
    }

    if ((uniform_flags & USE_NONE) != USE_NONE)
    {
        res = hc->device->CreateInputLayout(
            inputElements,
            ctr,
            vertexBC->GetBufferPointer(),
            vertexBC->GetBufferSize(),
            layout.GetAddressOf());
    }

}


