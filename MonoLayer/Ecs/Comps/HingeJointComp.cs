using System;
using SyEngine.Datas;
using SyEngine.Editor.Attributes;
using SyEngine.Helpers;

namespace SyEngine.Ecs.Comps
{
public struct HingeJointComp : SyEcs.IComp
{
	public SySceneEnt? AttachedToEnt;
	public SyVector3 PivotPos;
	public SyVector3 PivotRot;

	
	[Hide, NonSerialized]
	internal int? Hash;

	public override int GetHashCode()
		=> HashHelper.Combine(AttachedToEnt, PivotPos, PivotRot);
}
}