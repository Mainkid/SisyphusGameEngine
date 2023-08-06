#include "MeshComponent.h"
#include "../Core/EngineCore.h"

MeshComponent::MeshComponent()
{
    this->texturePath = L"Engine/Assets/DefaultTexture.png";
    this->modelPath = "Engine/Assets/Cube.fbx";
    Initialize();
}

MeshComponent::MeshComponent(std::string& modelPath, LPCWSTR& texturePath)
{
	this->texturePath = texturePath;
	this->modelPath = modelPath;
    Initialize();
}

bool MeshComponent::LoadModel()
{
    Assimp::Importer importer;
    
    const aiScene* pScene = importer.ReadFile(modelPath,aiProcess_Triangulate | 
        aiProcess_SortByPType | aiProcess_ConvertToLeftHanded);

    if (pScene == nullptr)
        return false;

    this->ProcessNode(pScene->mRootNode, pScene);


    return true;
}

void MeshComponent::LoadTexture()
{
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    HRESULT hr = EngineCore::instance()->device->CreateSamplerState(&sampDesc, this->samplerState.GetAddressOf()); //Create sampler state
    if (FAILED(hr))
    {
        std::cout << "Texture Loading Failed!" << std::endl;
    }
    
    //D3D11_TEXTURE2D
    //hr=DirectX::CreateWICTextureFromFileEx(game->device.Get(),texturePath,2000000,D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE,0,0,DirectX::WIC_LOADER_FORCE_SRGB,nullptr,texture.GetAddressOf());

    hr = DirectX::CreateWICTextureFromFile(EngineCore::instance()->device.Get(), texturePath, nullptr, texture.GetAddressOf());
    int q = 0;
}

void MeshComponent::UpdateMesh(std::string _modelPath, LPCWSTR _texturePath)
{
    meshes.clear();
    this->texturePath = _texturePath;
    this->modelPath = _modelPath;
    LoadModel();
    LoadTexture();
}

void MeshComponent::ProcessNode(aiNode* node, const aiScene* scene)
{
    for (UINT i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(this->ProcessMesh(mesh, scene));
    }

    for (UINT i = 0; i < node->mNumChildren; i++)
    {
        this->ProcessNode(node->mChildren[i], scene);
    }
}

std::shared_ptr<Mesh> MeshComponent::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<DirectX::SimpleMath::Vector4> vertices;

    std::vector<int> indices;

    for (UINT i = 0; i < mesh->mNumVertices; i++)
    {
        DirectX::XMFLOAT4 vertex;
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT4 normals = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        vertex.x = mesh->mVertices[i].x;
        vertex.y = mesh->mVertices[i].y;
        vertex.z = mesh->mVertices[i].z;
        vertex.w = 1.0f;

        if (mesh->mTextureCoords[0])
        {
            color.x = (float)mesh->mTextureCoords[0][i].x;
            color.y = (float)mesh->mTextureCoords[0][i].y;
        }

        normals.x = mesh->mNormals[i].x;
        normals.y = mesh->mNormals[i].y;
        normals.z = mesh->mNormals[i].z;


        vertices.push_back(vertex); //Вершины
        vertices.push_back(DirectX::XMFLOAT4(color.x, color.y, 1.0f, 1)); //UV-текстур
        vertices.push_back(normals); //Нормали

    }

    for (UINT i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];

        for (UINT j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    return  std::make_shared<Mesh>(vertices,indices);
}

void MeshComponent::Initialize()
{
	LoadModel();
	LoadTexture();
}

//TODO: Убрать sampDesc в отдельный файл, чтобы можно было создавать текстурки быстрее и проще.