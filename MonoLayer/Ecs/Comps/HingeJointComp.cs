using SyEngine.Datas;
using SyEngine.Helpers;

namespace SyEngine.Ecs.Comps
{
public struct HingeJointComp : SyEcs.IComp
{
	public int? AttachedToEnt;
	public SyVector3 PivotPos;
	public SyVector3 PivotRot;

	
	internal int? Hash;

	public override int GetHashCode()
		=> HashHelper.Combine(AttachedToEnt, PivotPos, PivotRot);
}
}