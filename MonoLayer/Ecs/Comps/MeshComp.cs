using System;
using System.Collections.Generic;
using SyEngine.Editor.Attributes;
using SyEngine.Helpers;
using SyEngine.Resources;

namespace SyEngine.Ecs.Comps
{
public struct MeshComp : SyEcs.IComp
{
	public List<ResRef<ResMaterial>> Materials;
	public ResRef<ResModel>          Model;

	
	[Hide, NonSerialized]
	internal int? Hash;

	public override int GetHashCode()
		=> HashHelper.Combine(Model, HashHelper.HashList(Materials));
}
}