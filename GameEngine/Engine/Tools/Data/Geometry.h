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
    //unite other geometry data with this one
    void Merge(const SyGeometry& other)
    {
        unsigned oldNumVertices = _vertices.size();
        _vertices.resize(oldNumVertices + other._vertices.size());
        for (unsigned i = 0; i < other._vertices.size(); i++)
            _vertices[oldNumVertices + i] = other._vertices[i];
        unsigned oldNumIndices = _indices.size();
        _indices.reserve(oldNumIndices + other._indices.size());
        for (unsigned i = 0; i < other._vertices.size() - 1; i++)
            AddSegment(oldNumVertices + i, oldNumVertices + i + 1);
        AddSegment(oldNumVertices + other._vertices.size() - 1, oldNumVertices);
    }
    void Translate(const SyVector3& translation)
    {
        for (unsigned i = 0; i < _vertices.size(); i++)
            _vertices[i].Translate(translation);
    }
    void Rotate(const SyVector3& eulerRotation)
    {
        for (unsigned i = 0; i < _vertices.size(); i++)
            _vertices[i].Rotate(eulerRotation);
    }
    //Construct 0-origin axis-aligned rectangle. Extent.z is ignored
    SyResult MakeRectangle(const SyVector3& extent)
    {
        _vertices.resize(4);
        _vertices[0] = SyVector3(extent.x, extent.y, 0.0f);
        _vertices[1] = _vertices[0].ReflectX();
        _vertices[2] = _vertices[0].ReflectY();
        _vertices[3] = _vertices[0].ReflectX().ReflectY();
        _indices.reserve(8);
        for (unsigned i = 0; i < 3; i++)
            AddSegment(i, i + 1);
        AddSegment(3, 0);
        return SyResult();
    }
    //Construct 0-origin axis-aligned box.
    SyResult MakeBox(const SyVector3& extent)
    {
       MakeRectangle(extent);
       for (unsigned i = 0; i < 4; i++)
           _vertices[i].z = extent.z;
       SyGeometry other;
       other.MakeRectangle(extent);
       for (unsigned i = 0; i < 4; i++)
           other._vertices[i].z = -extent.z;
       Merge(other);
       for (unsigned i = 0; i < 4; i++)
           AddSegment(i, i + 4);
       return SyResult();
    };
    //Construct a 0-origin round sector in xy-plane
    SyResult MakeSector(float radius, unsigned densityFactor = 1.0f, float startAngle  = 0.0f, float endAngle = 2.0f * SyMathHelper::PI)
    {
        SyResult result;
        if (densityFactor < 1)
        {
            result.code = SY_RESCODE_ERROR;
            result.message = xstring("Density factor for a half ring can't be less than 1.");
            SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Density factor for a half ring can't be less than 1.");
            return result;
        }
        unsigned numVertices = 4 * densityFactor; // densityFactor == 1 corresponds to square
        float angleStep = (endAngle - startAngle) / numVertices;
        _vertices.resize(numVertices);
        unsigned i = 0;
        for (float angle = startAngle; angle < endAngle; angle += angleStep)
        {
            _vertices[i] = SyVector3(radius * cosf(angle), radius * sinf(angle), 0.0f);
            i++;
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

