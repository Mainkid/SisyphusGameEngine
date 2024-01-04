using SyEngine.Datas;
using SyEngine.Ecs.Comps;

namespace SyEngine.Ecs.Sync
{
internal class SyEcsSyncRigid : SyEcsSyncBase<RigidComp, ProxyRigidComp>
{
	public SyEcsSyncRigid(SyEcs ecs) : base(ecs) { }

	public override EEngineCompId Id => EEngineCompId.Rigid;

	protected override void SendImpl(uint engineEnt, ref RigidComp rigid)
	{
		var proxy = new ProxyRigidComp
		{
			Type            = rigid.Type,
			Mass            = rigid.Mass,
			IsAutoMass      = rigid.IsAutoMass,
			IsKinematic     = rigid.IsKinematic,
			IsGravityOn     = rigid.IsGravityOn,
			LinearVelocity  = rigid.LinearVelocity,
			AngularVelocity = rigid.AngularVelocity
		};
		SyProxyEcs.GeUpdateRigidComp(engineEnt, proxy);
	}

	protected override void ReceiveImpl(ref ProxyRigidComp proxy, ref RigidComp rigid)
	{
		rigid.Type            = proxy.Type;
		rigid.Mass            = proxy.Mass;
		rigid.IsAutoMass      = proxy.IsAutoMass;
		rigid.IsKinematic     = proxy.IsKinematic;
		rigid.IsGravityOn     = proxy.IsGravityOn;
		rigid.LinearVelocity  = proxy.LinearVelocity;
		rigid.AngularVelocity = proxy.AngularVelocity;
	}

	protected override int? GetHashImpl(ref RigidComp comp)
		=> comp.Hash;

	protected override void SetHashImpl(ref RigidComp comp, int hash)
		=> comp.Hash = hash;
}
    
internal struct ProxyRigidComp
{
	public RigidComp.EType Type;
	public float           Mass;

	public bool IsAutoMass;
	public bool IsKinematic;
	public bool IsGravityOn;

	public SyVector3 LinearVelocity;
	public SyVector3 AngularVelocity;
}
}