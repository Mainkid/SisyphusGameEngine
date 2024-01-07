using SyEngine.Datas;
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

		if (joint.AttachedToEnt?.InternalEnt != null &&
		    Ecs.ToEngineEnt(joint.AttachedToEnt.Value.InternalEnt.Value, out uint attachedToEngineEnt))
		{
			proxy.IsAttachedToEnt     = true;
			proxy.AttachedToEngineEnt = attachedToEngineEnt;
		}
		else
			proxy.IsAttachedToEnt = false;
	}

	protected override void ReceiveImpl(ref ProxyFixedJointComp proxy, ref FixedJointComp joint)
	{
		if (proxy.IsAttachedToEnt && Ecs.ToGameEnt(proxy.AttachedToEngineEnt, out int gameEnt))
			joint.AttachedToEnt = new SySceneEnt(gameEnt);
		else
			joint.AttachedToEnt = null;
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