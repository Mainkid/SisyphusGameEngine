#pragma once
#include "Vector.h"
#include "../ErrorLogger.h"
#include "../MathHelper.h"
#include "../../Graphics/Buffer.h"
#include <memory>
#include <vector>


struct SyGeometry
{
    SyGeometry() = default;
    SyGeometry(const SyGeometry& other)
    {
        _vertices = std::vector<DirectX::SimpleMath::Vector4>(other._vertices.begin(), other._vertices.end());
        _indices = std::vector <unsigned>(other._indices.begin(), other._indices.end());
    };
    const std::vector<DirectX::SimpleMath::Vector4>& GetVertices() const
    {
        return _vertices;
    };
    const std::vector<unsigned>& GetIndices() const
    {
        return _indices;
    };
    void ResetBuffers()
    {
        //_vertexBuffer->buffer.Reset();
        _vertexBuffer.reset();
        _vertexBuffer = nullptr;
        //_indexBuffer->buffer.Reset();
        _indexBuffer.reset();
        _indexBuffer = nullptr;
    }
    Buffer* GetIndexBuffer()
    {
        if (_indexBuffer == nullptr)
        {
            _indexBuffer = std::make_shared<Buffer>();
            _indexBuffer->Initialize(_indices);
        }
        return _indexBuffer.get();
    }

    Buffer* GetVertexBuffer()
    {
        if (_vertexBuffer == nullptr)
        {
            _vertexBuffer = std::make_shared<Buffer>();
            _vertexBuffer->Initialize(_vertices);

        }
        return _vertexBuffer.get();
    }
    //unite other geometry data with this one
    void Merge(const SyGeometry& other)
    {
        unsigned oldNumVertices = _vertices.size();
        _vertices.insert(_vertices.end(), other._vertices.begin(), other._vertices.end());
        unsigned oldNumIndices = _indices.size();
        _indices.insert(_indices.end(), other._indices.begin(), other._indices.end());
        for (unsigned i = 0; i < other._indices.size(); i++)
            _indices[oldNumIndices + i] = other._indices[i] + oldNumVertices;
    }
    void Translate(const SyVector3& translation)
    {
        for (unsigned i = 0; i < _vertices.size(); i++)
        {
            _vertices[i].x += translation.x;
            _vertices[i].y += translation.y;
            _vertices[i].z += translation.z;
        };
    }
    void Rotate(const SyVector3& eulerRotation)
    {    
        for (unsigned i = 0; i < _vertices.size(); i++)
            _vertices[i] = DirectX::SimpleMath::Vector4::Transform(_vertices[i], DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(eulerRotation));
    }
    //Construct 0-origin axis-aligned rectangle in xy-plane. Extent.z is ignored
    void MakeRectangle(const SyVector3& extent)
    {
        _vertices.clear();
        _indices.clear();
        _vertices.resize(4);
        _vertices[0] = DirectX::SimpleMath::Vector4(extent.x, extent.y, 0.0f, 1.0f);
        _vertices[1] = DirectX::SimpleMath::Vector4(-extent.x, extent.y, 0.0f, 1.0f);
        _vertices[2] = DirectX::SimpleMath::Vector4(-extent.x, -extent.y, 0.0f, 1.0f);
        _vertices[3] = DirectX::SimpleMath::Vector4(extent.x, -extent.y, 0.0f, 1.0f);
        _indices.reserve(8);
        for (unsigned i = 0; i < 3; i++)
            AddSegment(i, i + 1);
        AddSegment(3, 0);
    }
    //Construct 0-origin axis-aligned box.
    void MakeBox(const SyVector3& extent)
    {
        _vertices.clear();
        _indices.clear();
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
    };
    //Construct a 0-origin round sector in xy-plane
    void MakeRingSector(float radius, unsigned densityFactor = 1.0f, float startAngle  = 0.0f, float endAngle = 2.0f * SyMathHelper::PI)
    {
        _vertices.clear();
        _indices.clear();
        if (densityFactor < 1)
        {
            SY_LOG_CORE(SY_LOGLEVEL_ERROR, "Density factor for a ring sector can't be less than 1.");
            return;
        }
        unsigned numVertices = 4 * densityFactor; 
        float angleStep = (endAngle - startAngle) / numVertices;
        if (endAngle - startAngle != 2 * SyMathHelper::PI) //add extra vertex if it's not a complete ring
            numVertices++;
        _vertices.resize(numVertices);
        
        for (unsigned i = 0; i < numVertices; i++)
        {
            _vertices[i] = DirectX::SimpleMath::Vector4(radius * cosf(angleStep * i), radius * sinf(angleStep * i), 0.0f, 1.0f);
        }
        _indices.reserve(2 * numVertices);
        for (unsigned i = 0; i < numVertices - 1; i++)
            AddSegment(i, i + 1);
        AddSegment(numVertices - 1, 0);
    }
    //Construct a 0-origin sphere wireframe
    void MakeSphere(float radius, unsigned densityFactor = 1.0f)
    {
        _vertices.clear();
        _indices.clear();
        if (densityFactor < 1)
        {
            SY_LOG_PHYS(SY_LOGLEVEL_ERROR, "Density factor for a sphere can't be less than 1.");
            return;
        }
        MakeRingSector(radius, densityFactor); //xy ring
        SyGeometry xzRing;
        xzRing.MakeRingSector(radius, densityFactor);
        xzRing.Rotate(SyVector3(SyMathHelper::PI / 2, 0.0f, 0.0f));
        Merge(xzRing);
        SyGeometry yzRing;
        yzRing.MakeRingSector(radius, densityFactor);
        yzRing.Rotate(SyVector3(0.0f, SyMathHelper::PI / 2, 0.0f));
        Merge(yzRing);
        return;
    }
    //Construct a 0-origin capsule wireframe
    void MakeCapsule(float radius, float halfHeight, unsigned densityFactor)
    {
        _vertices.clear();
        _indices.clear();
        if (densityFactor < 1)
        {
            SY_LOG_CORE(SY_LOGLEVEL_ERROR, "Density factor for a half ring can't be less than 1.");
            return;
        }
        MakeRectangle(SyVector3(radius, halfHeight / 2, 0.0f)); //upper half base of the capsule
        SyGeometry otherRect (*this);
        otherRect.Rotate(SyVector3(0.0f, SyMathHelper::PI / 2, 0.0f));
        Merge(otherRect);
        SyGeometry ring;
        ring.MakeRingSector(radius, densityFactor);
        ring.Rotate(SyVector3(SyMathHelper::PI / 2, 0.0f, 0.0f));
        ring.Translate(SyVector3(0.0f, halfHeight / 2, 0.0f));
        Merge(ring);
        Translate(SyVector3(0.0f, halfHeight / 2, 0.0f));
        SyGeometry ring2;
        ring2.MakeRingSector(radius, densityFactor);
        ring2.Rotate(SyVector3(SyMathHelper::PI / 2, 0.0f, 0.0f));
        Merge(ring2);
        SyGeometry xyHalfRing;
        xyHalfRing.MakeRingSector(radius, densityFactor, 0.0f, SyMathHelper::PI);
        xyHalfRing.Translate(SyVector3(0.0f, halfHeight, 0.0f));
        Merge(xyHalfRing);
        SyGeometry yzHalfRing;
        yzHalfRing.MakeRingSector(radius, densityFactor, 0.0f, SyMathHelper::PI);
        yzHalfRing.Rotate(SyVector3(0.0f, SyMathHelper::PI / 2, 0.0f));
        yzHalfRing.Translate(SyVector3(0.0f, halfHeight, 0.0f));
        Merge(yzHalfRing);
        SyGeometry bottomHalf(*this);
        bottomHalf.Rotate(SyVector3(SyMathHelper::PI, 0.0f, 0.0f));
        Merge(bottomHalf);
    }
private:
    void AddSegment(unsigned startI, unsigned endI)
    {
        _indices.push_back(startI);
        _indices.push_back(endI);
    }

    std::vector<DirectX::SimpleMath::Vector4> _vertices;
    std::vector<unsigned> _indices;

    std::shared_ptr<Buffer> _indexBuffer = nullptr;
    std::shared_ptr<Buffer> _vertexBuffer = nullptr;
};