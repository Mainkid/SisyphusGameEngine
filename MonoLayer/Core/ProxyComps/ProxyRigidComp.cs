using SyEngine.Core.Comps;
using SyEngine.Core.Datas;

namespace SyEngine.Core.ProxyComps
{
public struct ProxyRigidComp
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