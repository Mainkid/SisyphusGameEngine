#pragma once
#include "Vector.h"
#include <vector>
struct SyColliderGeometry
{
    const std::vector<SyVector3>& GetVertices() const
    {
        return _vertices;
    }
    const std::vector<unsigned>& GetIndices() const
    {
        return _indices;
    }
    void MakeBox(const SyVector3& extent)
    {
        _vertices.resize(8);
        _vertices[0] = extent;
        _vertices[1] = _vertices[0].ReflectX();
        _vertices[2] = _vertices[0].ReflectX();
        _vertices[3] = _vertices[0].ReflectX().ReflectY();
        for (unsigned i = 0; i < 4; i++)
            _vertices[i + 4] = _vertices[i].ReflectZ();
        _indices.reserve(24);
        for (unsigned i = 0; i < 3; i++)
            AddSegment(i, i + 1);
        AddSegment(3, 0);
        for (unsigned i = 0; i < 3; i++)
            AddSegment(i + 4, i + 5);
        AddSegment(7, 4);
        for (unsigned i = 0; i < 4; i++)
            AddSegment(i, i + 4);
    }
private:
    void AddSegment(unsigned startI, unsigned endI)
    {
        _indices.push_back(startI);
        _indices.push_back(endI);
    }
    
    std::vector<SyVector3> _vertices;
    std::vector<unsigned> _indices;
};

