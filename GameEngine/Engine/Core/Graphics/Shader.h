#pragma once
#include <windows.h>
#include <wrl.h>
#include <iostream>
#include <d3d11.h>
#include <d3dcompiler.h>

#include "Buffer.h"
#include "../Components/GameComponent.h"

class EngineCore;

class Shader 
{
public:
    
    virtual void Initialize(LPCWSTR shaderPath){};
    

	
    std::shared_ptr<Buffer> constantBuffer;
    std::shared_ptr<Buffer> indexBuffer;
    std::shared_ptr<Buffer> vertexBuffer;

    Microsoft::WRL::ComPtr<ID3D11InputLayout> layout;
    Microsoft::WRL::ComPtr<ID3DBlob> vertexBC; 
    Microsoft::WRL::ComPtr<ID3DBlob> pixelBC;
    Microsoft::WRL::ComPtr<ID3DBlob> geomBC;
    Microsoft::WRL::ComPtr<ID3DBlob> computeBC;
    Microsoft::WRL::ComPtr<ID3DBlob> domainBC;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader> geomShader;
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> computeShader;
    Microsoft::WRL::ComPtr<ID3D11DomainShader> domainShader;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rastState;

protected:

    Microsoft::WRL::ComPtr<ID3DBlob> errorPixelCode = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorVertexCode = nullptr;
    EngineCore* engine;
    
};

//TODO: убрать shared-ptr;