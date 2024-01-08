#pragma once
#include <vector>
#include <map>
#include <memory>
#include "SimpleMath.h"
#include "../../Core/Graphics/Buffer.h"
#include "../../Components/Material.h"
#include "../Animations/Bone.h"

class EngineCore;

//Simple Meshes
//Cube:
struct CubeMesh {
	static const std::vector<DirectX::SimpleMath::Vector4> verts;
	static const std::vector<int> indices;
};



using namespace DirectX::SimpleMath;
struct Mesh
{
public:
	Mesh(std::vector<DirectX::SimpleMath::Vector4> _vertices, std::vector<int> _indices);
	Mesh(const Mesh& mesh);
	Mesh() = default;

	std::vector<DirectX::SimpleMath::Vector4> vertices;
	std::vector<int> indices;
	std::shared_ptr<Buffer> indexBuffer;
	std::shared_ptr<Buffer> vertexBuffer;

	static const unsigned VECTORS4_PER_VERTEX = 7;
};


