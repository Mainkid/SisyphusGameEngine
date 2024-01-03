using SyEngine.Ecs.Comps;

namespace SyEngine.Ecs.Sync
{
internal class SyEcsSyncFixedJoint : SyEcsSyncBase<FixedJointComp, ProxyFixedJointComp>
{
	public SyEcsSyncFixedJoint(SyEcs ecs) : base(ecs) { }

	public override EEngineCompId Id => EEngineCompId.FixedJoint;

	protected override void SendImpl(uint engineEnt, ref FixedJointComp joint)
	{
		var proxy = new ProxyFixedJointComp();

		uint attachedToEngineEnt = default;
		proxy.IsAttachedToEnt = joint.AttachedToEnt != null &&
		                        _ecs.ToEngineEnt(joint.AttachedToEnt.Value, out attachedToEngineEnt);
		proxy.AttachedToEngineEnt = attachedToEngineEnt;
	}

	protected override void ReceiveImpl(ref ProxyFixedJointComp proxy, ref FixedJointComp joint)
	{
		joint.AttachedToEnt = proxy.IsAttachedToEnt &&
		                      _ecs.ToGameEnt(proxy.AttachedToEngineEnt, out int gameEnt)
			? (int?)gameEnt
			: null;
	}

	protected override int? GetHashImpl(ref FixedJointComp comp)
		=> comp.Hash;

	protected override void SetHashImpl(ref FixedJointComp comp, int hash)
		=> comp.Hash = hash;
}


internal struct ProxyFixedJointComp
{
	public bool IsAttachedToEnt;
	public uint AttachedToEngineEnt;
}
}