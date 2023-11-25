using SyEngine.Core.Datas;
using SyEngine.Core.Helpers;

namespace SyEngine.Core.Comps
{
public struct RigidComp : SyEcs.IComp
{
	public EType Type;
	public float Mass;

	public bool IsAutoMass;
	public bool IsKinematic;
	public bool IsGravityOn;

	public SyVector3 LinearVelocity;
	public SyVector3 AngularVelocity;


	internal int? Hash;

	public override int GetHashCode()
		=> HashHelper.Combine(
			Type,
			Mass,
			IsAutoMass,
			IsKinematic,
			IsGravityOn,
			LinearVelocity,
			AngularVelocity
		);


	public enum EType
	{
		Static,
		Dynamic
	}
}
}