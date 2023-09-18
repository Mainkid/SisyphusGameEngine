#pragma once
#include <memory>
#include <wrl.h>
#include <d3d11.h>
#include <directxmath.h>
#include <vector>

#include "SimpleMath.h"
//#include "ParticleSystem/Particle.h"

struct HardwareContext;

class Buffer
{
public:

    
    Buffer();
    Buffer(ID3D11Device* device);


   
    void Initialize(std::vector<int> points,D3D11_USAGE usage=D3D11_USAGE_DEFAULT,
    UINT bindFlags=D3D11_BIND_INDEX_BUFFER,UINT cpuAccess=0,
    UINT miscFlags=0,UINT stride=0,UINT memPitch=0, UINT memSlicePitch=0);

    
    void Initialize(std::vector<DirectX::SimpleMath::Vector4> points,D3D11_USAGE usage=D3D11_USAGE_DEFAULT,
  UINT bindFlags=D3D11_BIND_VERTEX_BUFFER,UINT cpuAccess=0,
  UINT miscFlags=0,UINT stride=0,UINT memPitch=0, UINT memSlicePitch=0);
   

    
    void  Initialize(size_t structSize,D3D11_USAGE usage=D3D11_USAGE_DYNAMIC,
    UINT bindFlags=D3D11_BIND_CONSTANT_BUFFER,UINT cpuAccess=D3D11_CPU_ACCESS_WRITE,
    UINT miscFlags=0,UINT stride=0,UINT memPitch=0, UINT memSlicePitch=0);


    void Initialize(void* data, int _size,int dataStructureSize,bool isUAV);
    
    void InitializeIndirect(int capacity);

    void InitializeCounterBuffer();
    


    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
    size_t size=0;
  
private:
    HardwareContext* hc;
    
};
