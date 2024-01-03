using System;
using SyEngine.Datas;
using SyEngine.Editor.Attributes;

namespace SyEngine.Ecs.Comps
{
public struct FixedJointComp : SyEcs.IComp
{
	public SySceneEnt? AttachedToEnt;

	[Hide, NonSerialized]
	internal int? Hash;
	
	public override int GetHashCode()
		=> AttachedToEnt.GetHashCode();
}
}