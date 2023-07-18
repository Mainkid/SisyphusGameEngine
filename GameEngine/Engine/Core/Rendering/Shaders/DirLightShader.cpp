#include "DirLightShader.h"
#include "../../EngineCore.h"


DirLightShader::DirLightShader(EngineCore* _engine)
{
    this->engine=_engine;
}

void DirLightShader::Initialize(LPCWSTR shaderPath)
{
    D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr };
    
    HRESULT res = D3DCompileFromFile(shaderPath,
        nullptr /*macros*/,
        nullptr /*include*/,
        "VSMain",
        "vs_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        vertexBC.GetAddressOf(),
        errorVertexCode.GetAddressOf());
    
    if (FAILED(res)) {
        // If the shader failed to compile it should have written something to the error message.
        if (errorVertexCode) {
            char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());

            std::cout << compileErrors << std::endl;
        }
        // If there was  nothing in the error message then it simply could not find the shader file itself.
        else
        {
            MessageBox(engine->window->GetHWND(), L"./Shaders/Racket.hlsl", L"Missing Shader File", MB_OK);
        }

    } 

    res = D3DCompileFromFile(shaderPath, Shader_Macros /*macros*/, nullptr /*include*/,
        "PS_Directional", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0, pixelBC.GetAddressOf(), errorPixelCode.GetAddressOf());
    


    res = engine->device->CreateVertexShader(
        this->vertexBC->GetBufferPointer(),
        this->vertexBC->GetBufferSize(),
        nullptr, this->vertexShader.GetAddressOf());

    res = engine->device->CreatePixelShader(
        this->pixelBC->GetBufferPointer(),
        this->pixelBC->GetBufferSize(),
        nullptr, this->pixelShader.GetAddressOf());
    

    D3D11_INPUT_ELEMENT_DESC inputElements[] = {
        D3D11_INPUT_ELEMENT_DESC {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            0,
            D3D11_INPUT_PER_VERTEX_DATA,
            0},
        D3D11_INPUT_ELEMENT_DESC {
            "COLOR",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0}
    };

    res = engine->device->CreateInputLayout(
        inputElements,
        2,
        vertexBC->GetBufferPointer(),
        vertexBC->GetBufferSize(),
        layout.GetAddressOf());
}

