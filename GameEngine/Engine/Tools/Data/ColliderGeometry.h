#pragma once
#include "Vector.h"
#include "../ErrorLogger.h"
#include "../MathHelper.h"
#include <vector>

struct SyGeometry
{
    const std::vector<SyVector3>& GetVertices() const
    {
        return _vertices;
    };
    const std::vector<unsigned>& GetIndices() const
    {
        return _indices;
    };

    
    SyResult MakeBox(const SyVector3& extent)
    {
        _vertices.resize(8);
        _vertices[0] = extent;
        _vertices[1] = _vertices[0].ReflectX();
        _vertices[2] = _vertices[0].ReflectY();
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
        return SyResult();
    };
    SyResult MakeHalfRing(const SyVector3& radiusVector, unsigned densityFactor)
    {
        SyResult result;
        if (densityFactor < 1)
        {
            result.code = SY_RESCODE_ERROR;
            result.message = xstring("Density factor for a half ring can't be less than 1.");
            SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Density factor for a half ring can't be less than 1.");
            return result;
        }
        unsigned numVertices = 3 + 2 * (densityFactor - 1);
        float angleStep = SyMathHelper::PI / (numVertices - 1);
        _vertices.resize(numVertices);
        _vertices[numVertices / 2] = radiusVector;
        for (unsigned i = 1; i < numVertices / 2 + 1; i++)
        {
            float x1 = radiusVector.x - sin(i * angleStep);
            float x2 = radiusVector.x + sin(i * angleStep);
            float y = radiusVector.y  - cos(i * angleStep) * SyMathHelper::Sign(radiusVector.y);
            float z = radiusVector.z;
            _vertices[numVertices / 2 + i] = SyVector3(x1, y, z);
            _vertices[numVertices / 2 - i] = SyVector3(x2, y, z);
        }
        _indices.reserve(2 * numVertices);
        for (unsigned i = 0; i < numVertices - 1; i++)
            AddSegment(i, i + 1);
        AddSegment(numVertices - 1, 0);
        return SyResult();
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

