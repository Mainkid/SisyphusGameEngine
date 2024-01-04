using System;
using SyEngine.Editor.Attributes;
using SyEngine.Resources;

namespace SyEngine.Ecs.Comps
{
public struct SkyboxComp : SyEcs.IComp
{
	public ResRef<ResCubemap> Cubemap;

	[Hide, NonSerialized]
	internal int? Hash;

	public override int GetHashCode()
		=> Cubemap?.GetHashCode() ?? 0;
}
}