using SyEngine.Datas;
using SyEngine.Ecs.Comps;

namespace SyEngine.Ecs.Sync
{
internal class SyEcsSyncNavMesh : SyEcsSyncBase<NavMeshComp, ProxyNavMeshComp>
{
	public SyEcsSyncNavMesh(SyEcs ecs) : base(ecs) { }
	
	public override EEngineCompId Id => EEngineCompId.NavMesh;

	protected override void SendImpl(uint engineEnt, ref NavMeshComp comp)
	{
		var proxy = new ProxyNavMeshComp
		{
			Origin                     = comp.Origin,
			Extent                     = comp.Extent,
			CellSize                   = comp.CellSize,
			CellHeight                 = comp.CellHeight,
			AgentMaxWalkableSlopeAngle = comp.AgentMaxWalkableSlopeAngle,
			AgentHeight                = comp.AgentHeight,
			AgentMaxClimb              = comp.AgentMaxClimb,
			AgentRadius                = comp.AgentRadius,
			EdgeMaxError               = comp.EdgeMaxError,
			EdgeMaxLen                 = comp.EdgeMaxLen,
			RegionMinSize              = comp.RegionMinSize,
			RegionMergeSize            = comp.RegionMergeSize,
			MaxVertsPerPoly            = comp.MaxVertsPerPoly,
			DetailSampleDist           = comp.DetailSampleDist,
			DetailSampleMaxError       = comp.DetailSampleMaxError,
			PartitioningType           = comp.PartitioningType,
		};
		SyProxyEcs.GeUpdateNavMeshComp(engineEnt, proxy);
	}

	protected override void ReceiveImpl(ref ProxyNavMeshComp proxy, ref NavMeshComp comp)
	{
		comp.Origin                     = proxy.Origin;
		comp.Extent                     = proxy.Extent;
		comp.CellSize                   = proxy.CellSize;
		comp.CellHeight                 = proxy.CellHeight;
		comp.AgentMaxWalkableSlopeAngle = proxy.AgentMaxWalkableSlopeAngle;
		comp.AgentHeight                = proxy.AgentHeight;
		comp.AgentMaxClimb              = proxy.AgentMaxClimb;
		comp.AgentRadius                = proxy.AgentRadius;
		comp.EdgeMaxError               = proxy.EdgeMaxError;
		comp.EdgeMaxLen                 = proxy.EdgeMaxLen;
		comp.RegionMinSize              = proxy.RegionMinSize;
		comp.RegionMergeSize            = proxy.RegionMergeSize;
		comp.MaxVertsPerPoly            = proxy.MaxVertsPerPoly;
		comp.DetailSampleDist           = proxy.DetailSampleDist;
		comp.DetailSampleMaxError       = proxy.DetailSampleMaxError;
		comp.PartitioningType           = proxy.PartitioningType;
	}

	protected override int? GetHashImpl(ref NavMeshComp comp)
		=> 0;

	protected override void SetHashImpl(ref NavMeshComp comp, int hash)
		=> comp.IsDirty = false;
}
    
public struct ProxyNavMeshComp
{
	public SyVector3 Origin;
	public SyVector3 Extent;
	public float     CellSize;
	public float     CellHeight;
	public float     AgentMaxWalkableSlopeAngle;
	public float     AgentHeight;
	public float     AgentMaxClimb;
	public float     AgentRadius;
	public float     EdgeMaxError;
	public float     EdgeMaxLen;
	public float     RegionMinSize;
	public float     RegionMergeSize;
	public int       MaxVertsPerPoly;
	public float     DetailSampleDist;
	public float     DetailSampleMaxError;

	public NavMeshComp.EPartitionType PartitioningType;
}
}