#include "Buffer.h"

#include "../ServiceLocator.h"
#include "../../Contexts/HardwareContext.h"

Buffer::Buffer()
{
    hc = ServiceLocator::instance()->Get<HardwareContext>();
}

Buffer::Buffer(ID3D11Device* _device)
{
    hc = ServiceLocator::instance()->Get<HardwareContext>();
}


void Buffer::Initialize(std::vector<DirectX::SimpleMath::Vector4> points,D3D11_USAGE usage,
  UINT bindFlags,UINT cpuAccess,
  UINT miscFlags,UINT stride,UINT memPitch, UINT memSlicePitch)
{

    
    D3D11_BUFFER_DESC indexBufDesc = {};
    indexBufDesc.Usage = usage;
    indexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    indexBufDesc.CPUAccessFlags = cpuAccess;
    indexBufDesc.MiscFlags = miscFlags;
    indexBufDesc.StructureByteStride = stride;
    indexBufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * points.size();
        
    D3D11_SUBRESOURCE_DATA indexData = {};
        
    indexData.pSysMem = static_cast<void*>(points.data());
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    size=sizeof(DirectX::XMFLOAT4) * points.size();
    //this->stride=stride;
    //this->offset=offset;
    HRESULT res = hc->device->CreateBuffer(&indexBufDesc, &indexData, buffer.GetAddressOf());
}

void Buffer::Initialize(std::vector<SyVector3> points, D3D11_USAGE usage, UINT bindFlags, UINT cpuAccess,
	UINT miscFlags, UINT stride, UINT memPitch, UINT memSlicePitch)
{

    D3D11_BUFFER_DESC indexBufDesc = {};
    indexBufDesc.Usage = usage;
    indexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    indexBufDesc.CPUAccessFlags = cpuAccess;
    indexBufDesc.MiscFlags = miscFlags;
    indexBufDesc.StructureByteStride = stride;
    indexBufDesc.ByteWidth = sizeof(SyVector3) * points.size();

    D3D11_SUBRESOURCE_DATA indexData = {};

    indexData.pSysMem = static_cast<void*>(points.data());
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    size = sizeof(SyVector3) * points.size();
    //this->stride=stride;
    //this->offset=offset;
    HRESULT res = hc->device->CreateBuffer(&indexBufDesc, &indexData, buffer.GetAddressOf());
}


void Buffer::Initialize(std::vector<int> points,D3D11_USAGE usage,
                        UINT bindFlags,UINT cpuAccess,
                        UINT miscFlags,UINT stride,UINT memPitch, UINT memSlicePitch)
{
    D3D11_BUFFER_DESC indexBufDesc = {};
    indexBufDesc.Usage = usage;
    indexBufDesc.BindFlags = bindFlags;
    indexBufDesc.CPUAccessFlags = cpuAccess;
    indexBufDesc.MiscFlags = miscFlags;
    indexBufDesc.StructureByteStride = stride;
    indexBufDesc.ByteWidth = sizeof(int) * points.size();
        
    D3D11_SUBRESOURCE_DATA indexData = {};
        
    indexData.pSysMem = static_cast<void*>(points.data());
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    size= points.size();
   
    HRESULT res = hc->device->CreateBuffer(&indexBufDesc, &indexData, buffer.GetAddressOf());
}

void Buffer::Initialize(std::vector<unsigned> points, D3D11_USAGE usage,
    UINT bindFlags, UINT cpuAccess,
    UINT miscFlags, UINT stride, UINT memPitch, UINT memSlicePitch)
{
    D3D11_BUFFER_DESC indexBufDesc = {};
    indexBufDesc.Usage = usage;
    indexBufDesc.BindFlags = bindFlags;
    indexBufDesc.CPUAccessFlags = cpuAccess;
    indexBufDesc.MiscFlags = miscFlags;
    indexBufDesc.StructureByteStride = stride;
    indexBufDesc.ByteWidth = sizeof(int) * points.size();

    D3D11_SUBRESOURCE_DATA indexData = {};

    indexData.pSysMem = static_cast<void*>(points.data());
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    size = points.size();

    HRESULT res = hc->device->CreateBuffer(&indexBufDesc, &indexData, buffer.GetAddressOf());
}

void  Buffer::Initialize(size_t structSize,D3D11_USAGE usage,
    UINT bindFlags,UINT cpuAccess,
    UINT miscFlags,UINT stride,UINT memPitch, UINT memSlicePitch)
{
    D3D11_BUFFER_DESC constantBufDesc = {};
    constantBufDesc.Usage = usage;
    constantBufDesc.BindFlags = bindFlags;
    constantBufDesc.CPUAccessFlags = cpuAccess;
    constantBufDesc.MiscFlags = 0;

    constantBufDesc.ByteWidth = structSize + (16 - structSize) % 16;
    constantBufDesc.StructureByteStride = 0;

    size=structSize + (16 - structSize) % 16;
    //this->stride=stride;
    //this->offset=offset;
    HRESULT res= hc->device->CreateBuffer(&constantBufDesc, 0, buffer.GetAddressOf());
}


void Buffer::Initialize(void* data, int _size,int dataStructureSize,bool isUAV)
{
    D3D11_BUFFER_DESC structuredBufDesc = {};
    structuredBufDesc.Usage = D3D11_USAGE_DEFAULT;
    structuredBufDesc.ByteWidth = dataStructureSize * _size;
    structuredBufDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    structuredBufDesc.CPUAccessFlags = 0;
    structuredBufDesc.StructureByteStride = dataStructureSize; //???
    structuredBufDesc.MiscFlags =D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem =data;
    
    
    HRESULT res=0;
    size = _size;
    if (data!=nullptr)
        res= hc->device->CreateBuffer(&structuredBufDesc,&indexData, buffer.GetAddressOf());
    else
        res= hc->device->CreateBuffer(&structuredBufDesc,nullptr, buffer.GetAddressOf());
    int test=0;
    
}

void Buffer::InitializeIndirect(int capacity)
{
    struct DrawArgs {
        UINT IndexCountPerInstance;
        UINT InstanceCount;
        UINT StartIndexLocation;
        INT  BaseVertexLocation;
        UINT StartInstanceLocation;
    };
    
    D3D11_BUFFER_DESC structuredBufDesc = {};
    structuredBufDesc.Usage = D3D11_USAGE_DEFAULT;
    structuredBufDesc.ByteWidth = sizeof(DrawArgs);
    structuredBufDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    structuredBufDesc.CPUAccessFlags = 0;
    structuredBufDesc.StructureByteStride = 4; //???
    structuredBufDesc.MiscFlags =D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;

    DrawArgs d_args;
    d_args.IndexCountPerInstance=6*capacity;
    d_args.InstanceCount=1;
    d_args.StartIndexLocation=0;
    d_args.StartInstanceLocation=0;
    d_args.BaseVertexLocation=0;
  
    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem =&d_args;
    
    
    HRESULT res=0;
    size = sizeof(DrawArgs);
     res= hc->device->CreateBuffer(&structuredBufDesc,&indexData, buffer.GetAddressOf());

    int test=0;
    
}

void Buffer::InitializeCounterBuffer()
{
    D3D11_BUFFER_DESC structuredBufDesc = {};
    structuredBufDesc.Usage = D3D11_USAGE_DEFAULT;
    structuredBufDesc.ByteWidth = 4;
    structuredBufDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS|D3D11_BIND_SHADER_RESOURCE;
    structuredBufDesc.CPUAccessFlags =0;
    structuredBufDesc.StructureByteStride = 4; 
    structuredBufDesc.MiscFlags = 0;
    
    
    
    HRESULT res=0;
    size = 1;
    res= hc->device->CreateBuffer(&structuredBufDesc,nullptr, buffer.GetAddressOf());

    int test=0;
}
