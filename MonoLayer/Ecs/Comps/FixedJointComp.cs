namespace SyEngine.Ecs.Comps
{
public struct FixedJointComp : SyEcs.IComp
{
	public int? AttachedToEnt;

	internal int? Hash;

	public override int GetHashCode()
		=> AttachedToEnt.GetHashCode();
}
}