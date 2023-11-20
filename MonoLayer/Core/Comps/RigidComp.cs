namespace SyEngine.Core.Comps
{
public struct RigidComp : SyEcs.IComp
{
	public EType Type;
	public float Mass;
	

	public enum EType
	{
		Static,
		Dynamic
	}
}
}