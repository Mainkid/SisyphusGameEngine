#pragma once
#include "Recast.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"
#include <memory>
#include "../../Core/Graphics/Buffer.h"

/// Mask of the ceil part of the area id (3 lower bits)
/// the 0 value (RC_NULL_AREA) is left unused
static const unsigned char SAMPLE_POLYAREA_TYPE_MASK = 0x07;
/// Value for the kind of ceil "ground"
static const unsigned char SAMPLE_POLYAREA_TYPE_GROUND = 0x1;
/// Value for the kind of ceil "water"
static const unsigned char SAMPLE_POLYAREA_TYPE_WATER = 0x2;
/// Value for the kind of ceil "road"
static const unsigned char SAMPLE_POLYAREA_TYPE_ROAD = 0x3;
/// Value for the kind of ceil "grass"
static const unsigned char SAMPLE_POLYAREA_TYPE_GRASS = 0x4;
/// Flag for door area. Can be combined with area types and jump flag.
static const unsigned char SAMPLE_POLYAREA_FLAG_DOOR = 0x08;
/// Flag for jump area. Can be combined with area types and door flag.
static const unsigned char SAMPLE_POLYAREA_FLAG_JUMP = 0x10;

enum ESyNavMeshPartitionType
{
    Watershed,
    Monotone,
    Layer
};

struct SyNavMeshComponent
{
public:
    //---- User vars ----
    SyVector3 origin = SyVector3::ZERO;        //center of navmesh AABB
    SyVector3 extent = SyVector3::ONE;         //extent of navmesh AABB 
    float cellSize = 0.3f;                     //xz-plane cell size
    float cellHeight = 0.2f;                   //y-axis cell size
    float agentMaxWalkableSlopeAngle = 45.0f;  //maximum slope angle that is considered walkable (degrees)
    float agentHeight = 2.0f;                  //minimal floor-ceiling distance which allows agent to walk over the floor
    float agentMaxClimb = 0.9f;                //maximum ledge height for it to be traversable
    float agentRadius = 0.6f;                  //
    float edgeMaxError = 1.3f;
    float edgeMaxLen = 12.0f;
    float regionMinSize = 8.0f;
    float regionMergeSize = 20.0f;
    int   maxVertsPerPoly = 6;
    float detailSampleDist = 6.0f;
    float detailSampleMaxError = 1.0f;
    
    ESyNavMeshPartitionType partitioningType;
private:
    //---- Engine vars ----
    //---- Scene Mesh vars
    unsigned numVerts;
    std::vector<float> vertices;
    unsigned numTris;
    std::vector<int> triangles;
    std::vector<unsigned char> triAreaTypes;

    //---- Recast navmesh vars ----
    rcConfig buildConfig;
    std::shared_ptr<rcContext> recastContext = std::make_shared<rcContext>();
    std::shared_ptr<rcHeightfield> heightfield;
    std::shared_ptr<rcCompactHeightfield> compactHeightfield;
    std::shared_ptr<rcContourSet> contourSet;
    std::shared_ptr<rcPolyMesh> mesh;
    std::shared_ptr<rcPolyMeshDetail> meshDetail;
    rcAreaModification const SAMPLE_AREAMOD_GROUND = rcAreaModification(SAMPLE_POLYAREA_TYPE_GROUND, SAMPLE_POLYAREA_TYPE_MASK);;

    //---- Detour vars
    std::shared_ptr<dtNavMesh> navMesh;
    std::shared_ptr<dtNavMeshQuery> navQuery = std::make_shared<dtNavMeshQuery>();
    static const int MAX_VERTS_PER_POLYGON = 6;

    //---- Debug draw vars
    std::shared_ptr<Buffer> _aabbVertexBuffer = std::make_shared<Buffer>();
    std::shared_ptr<Buffer> _aabbIndexBuffer = std::make_shared<Buffer>();
    bool _wasMeshDataPrepared = true;
    std::shared_ptr<Buffer> _meshVertexBuffer = std::make_shared<Buffer>();
    bool _wasIntEdgesDataPrepared = true;
    std::shared_ptr<Buffer> _internalEdgesVertexBuffer = std::make_shared<Buffer>();
    bool _wasExtEdgesDataPrepared = true;
    std::shared_ptr<Buffer> _externalEdgesVertexBuffer = std::make_shared<Buffer>();
    
    friend class SyNavMeshSystem;
    friend class SyNavMeshDrawSystem;
};