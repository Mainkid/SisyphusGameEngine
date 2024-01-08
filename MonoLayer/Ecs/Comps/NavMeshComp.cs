using System;
using SyEngine.Datas;
using SyEngine.Editor.Attributes;

namespace SyEngine.Ecs.Comps
{
public struct NavMeshComp : SyEcs.IComp
{
	public SyVector3      Origin;
	public SyVector3      Extent;
	public float          CellSize;
	public float          CellHeight;
	public float          AgentMaxWalkableSlopeAngle;
	public float          AgentHeight;
	public float          AgentMaxClimb;
	public float          AgentRadius;
	public float          EdgeMaxError;
	public float          EdgeMaxLen;
	public float          RegionMinSize;
	public float          RegionMergeSize;
	public int            MaxVertsPerPoly;
	public float          DetailSampleDist;
	public float          DetailSampleMaxError;
	public EPartitionType PartitioningType;

    
	[Hide, NonSerialized]
	public bool IsDirty;

	public override int GetHashCode()
		=> IsDirty ? 1 : 0;


	public enum EPartitionType
	{
		Watershed,
		Monotone,
		Layer
	}
}
}