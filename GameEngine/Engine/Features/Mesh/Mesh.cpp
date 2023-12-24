#include "Mesh.h"
#include "../../Core/EngineCore.h"
#include "../../Components/Material.h"



Mesh::Mesh( std::vector<DirectX::SimpleMath::Vector4> _vertices, std::vector<int> _indices)
{

    this->vertices = _vertices;
    this->indices = _indices;

    indexBuffer = std::make_shared<Buffer>();
    indexBuffer->Initialize(indices);

    vertexBuffer = std::make_shared<Buffer>();
    vertexBuffer->Initialize(vertices);
}

Mesh::Mesh(const Mesh& mesh)
{
    this->vertices = mesh.vertices;
    this->indices = mesh.indices;
    this->vertexBuffer = mesh.vertexBuffer;
    this->indexBuffer = mesh.indexBuffer;
}

//const std::vector<int> CubeMesh::indices = {
//	2, 6, 7,
//		2, 3, 7,
//
//		//Bottom
//		0, 4, 5,
//		0, 1, 5,
//
//		//Left
//		0, 2, 6,
//		0, 4, 6,
//
//		//Right
//		1, 3, 7,
//		1, 5, 7,
//
//		//Front
//		0, 2, 3,
//		0, 1, 3,
//
//		//Back
//		4, 6, 7,
//		4, 5, 7
//};
//
//const std::vector<DirectX::SimpleMath::Vector4> CubeMesh::verts = {
//		{-1, -1,  1,1 }, //0
//		{1, -1,  1,1}, //1
//		{-1,  1,  1,1 }, //2
//		{1,  1,  1,1}, //3
//		{-1, -1, -1,1 }, //4
//		{1, -1, -1,1}, //5
//		{-1,  1, -1,1 }, //6
//		{1,  1, -1,1}  //7
//};