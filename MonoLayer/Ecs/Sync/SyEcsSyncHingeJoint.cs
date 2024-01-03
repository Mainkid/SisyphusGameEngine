using SyEngine.Datas;
using SyEngine.Ecs.Comps;

namespace SyEngine.Ecs.Sync
{
internal class SyEcsSyncHingeJoint : SyEcsSyncBase<HingeJointComp, ProxyHingeJointComp>
{
	public SyEcsSyncHingeJoint(SyEcs ecs) : base(ecs) { }

	public override EEngineCompId Id => EEngineCompId.HingeJoint;

	protected override void SendImpl(uint engineEnt, ref HingeJointComp joint)
	{
		var proxy = new ProxyHingeJointComp();
        
		if (joint.AttachedToEnt?.InternalEnt != null &&
		    Ecs.ToEngineEnt(joint.AttachedToEnt.Value.InternalEnt.Value, out uint attachedToEngineEnt))
		{
			proxy.IsAttachedToEnt     = true;
			proxy.AttachedToEngineEnt = attachedToEngineEnt;
		}
		else
			proxy.IsAttachedToEnt = false;

		proxy.PivotPos = joint.PivotPos;
		proxy.PivotRot = joint.PivotRot;
	}

	protected override void ReceiveImpl(ref ProxyHingeJointComp proxy, ref HingeJointComp joint)
	{
		if (proxy.IsAttachedToEnt && Ecs.ToGameEnt(proxy.AttachedToEngineEnt, out int gameEnt))
			joint.AttachedToEnt = new SySceneEnt(gameEnt);
		else
			joint.AttachedToEnt = null;

		joint.PivotPos = proxy.PivotPos;
		joint.PivotRot = proxy.PivotRot;
	}

	protected override int? GetHashImpl(ref HingeJointComp comp)
		=> comp.Hash;

	protected override void SetHashImpl(ref HingeJointComp comp, int hash)
		=> comp.Hash = hash;
}

internal struct ProxyHingeJointComp
{
	public bool IsAttachedToEnt;
	public uint AttachedToEngineEnt;

	public SyVector3 PivotPos;
	public SyVector3 PivotRot;
}
}