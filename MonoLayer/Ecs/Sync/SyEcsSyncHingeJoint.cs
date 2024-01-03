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
        
		uint attachedToEngineEnt = default;
		proxy.IsAttachedToEnt = joint.AttachedToEnt != null &&
		                        _ecs.ToEngineEnt(joint.AttachedToEnt.Value, out attachedToEngineEnt);
		proxy.AttachedToEngineEnt = attachedToEngineEnt;

		proxy.PivotPos = joint.PivotPos;
		proxy.PivotRot = joint.PivotRot;
	}

	protected override void ReceiveImpl(ref ProxyHingeJointComp proxy, ref HingeJointComp joint)
	{
		joint.AttachedToEnt = proxy.IsAttachedToEnt &&
		                      _ecs.ToGameEnt(proxy.AttachedToEngineEnt, out int gameEnt)
			? (int?)gameEnt
			: null;

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