#pragma once
#include "Recast.h"
#include <memory>

enum ESyNavMeshPartitionType
{
    Watershed,
    Monotone,
    Layer
};

struct SyNavMeshComponent
{
private:
    unsigned char* triAreas;
    rcHeightfield* heightfield;
    std::shared_ptr<rcCompactHeightfield> compactHeightfield;
    std::shared_ptr<rcContourSet> contourSet;
    std::shared_ptr<rcPolyMesh> mesh;
    rcConfig buildConfig;	
    std::shared_ptr<rcPolyMeshDetail> meshDetail;
    rcContext* recastContext;
    ESyNavMeshPartitionType partitioningType;
    
    friend class SyNavMeshSystem;
};