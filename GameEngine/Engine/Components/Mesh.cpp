#include "Mesh.h"
#include "../Core/EngineCore.h"

Mesh::Mesh(EngineCore* _engine, std::vector<DirectX::SimpleMath::Vector4> _vertices, std::vector<int> _indices)
{

    this->engine = _engine;
    this->vertices = _vertices;
    this->indices = _indices;

    indexBuffer = std::make_shared<Buffer>(engine->device.Get());
    indexBuffer->Initialize(indices);

    vertexBuffer = std::make_shared<Buffer>(engine->device.Get());
    vertexBuffer->Initialize(vertices);
}

Mesh::Mesh(const Mesh& mesh)
{
    this->vertices = mesh.vertices;
    this->indices = mesh.indices;
    this->vertexBuffer = mesh.vertexBuffer;
    this->indexBuffer = mesh.indexBuffer;
    this->engine = mesh.engine;
}