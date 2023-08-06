#pragma once
#include <vector>
#include <memory>
#include "../DirectXSDK/SimpleMath.h"
#include "../Core/Graphics/Buffer.h"

class EngineCore;

//Simple Meshes
//Cube:
static struct CubeMesh {
	static const std::vector<DirectX::SimpleMath::Vector4> verts;
	static const std::vector<int> indices;
};

using namespace DirectX::SimpleMath;
class Mesh
{
public:
	Mesh(std::vector<DirectX::SimpleMath::Vector4> _vertices, std::vector<int> _indices);
	Mesh(const Mesh& mesh);

	~Mesh()
	{

	}

	std::vector<DirectX::SimpleMath::Vector4> vertices;
	std::vector<int> indices;
	std::shared_ptr<Buffer> indexBuffer;
	std::shared_ptr<Buffer> vertexBuffer;
};


