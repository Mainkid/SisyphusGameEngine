using System;
using SyEngine.Editor.Attributes;
using SyEngine.Helpers;

namespace SyEngine.Ecs.Comps
{
public struct SceneObjectComp : SyEcs.IInternalComp
{
	public string Name;
	public bool   IsActive;

	[Hide, NonSerialized]
	internal int? Hash;

	public override int GetHashCode()
		=> HashHelper.Combine(Name, IsActive);
}
}