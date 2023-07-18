#pragma once
#include <windows.h>
#include <wrl.h>
#include <iostream>
#include <d3d11.h>
#include <d3dcompiler.h>

#include "Buffer.h"
#include "../Components/GameComponent.h"

#define COMPILE_PIXEL 1
#define COMPILE_VERTEX 2
#define COMPILE_GEOM 4
#define COMPILE_COMPUTE 8

class EngineCore;

class Shader 
{
public:
    
    virtual void Initialize(LPCWSTR shaderPath){};
    

	
    std::unique_ptr<Buffer> constantBuffer;
    std::unique_ptr<Buffer> indexBuffer;
    std::unique_ptr<Buffer> vertexBuffer;

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
    
};

//TODO: убрать shared-ptr;