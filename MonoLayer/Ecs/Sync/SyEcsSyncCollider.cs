using SyEngine.Datas;
using SyEngine.Ecs.Comps;

namespace SyEngine.Ecs.Sync
{
internal class SyEcsSyncCollider : SyEcsSyncBase<ColliderComp, ProxyColliderComp>
{
	public SyEcsSyncCollider(SyEcs ecs) : base(ecs) { }

	public override EEngineCompId Id => EEngineCompId.Collider;

	protected override void SendImpl(uint engineEnt, ref ColliderComp collider)
	{
		var proxy = new ProxyColliderComp
		{
			Type       = collider.Type,
			Extent     = collider.Extent,
			Radius     = collider.Radius,
			HalfHeight = collider.HalfHeight
		};
		SyProxyEcs.GeUpdateColliderComp(engineEnt, proxy);
	}

	protected override void ReceiveImpl(ref ProxyColliderComp proxy, ref ColliderComp collider)
	{
		collider.Type       = proxy.Type;
		collider.Extent     = proxy.Extent;
		collider.Radius     = proxy.Radius;
		collider.HalfHeight = proxy.HalfHeight;
	}

	protected override int? GetHashImpl(ref ColliderComp comp)
		=> comp.Hash;

	protected override void SetHashImpl(ref ColliderComp comp, int hash)
		=> comp.Hash = hash;
}


internal struct ProxyColliderComp
{
	public ColliderComp.EType Type;
	public SyVector3          Extent;
	public float              Radius;
	public float              HalfHeight;
}
}