﻿using SyEngine.Core.Resources;

namespace SyEngine.Core.Comps
{
public struct SkyboxComp : SyEcs.IComp
{
	public ResRef<ResCubemap> Cubemap;

	internal int? Hash;

	public override int GetHashCode()
		=> Cubemap?.GetHashCode() ?? 0;
}
}