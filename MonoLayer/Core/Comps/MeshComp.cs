using System.Collections.Generic;
using SyEngine.Core.Helpers;
using SyEngine.Core.Resources;

namespace SyEngine.Core.Comps
{
public struct MeshComp : SyEcs.IComp
{
	public List<ResRef<ResMaterial>> Materials;
	public ResRef<ResModel>          Model;

	internal int Hash;

	public override int GetHashCode()
		=> HashHelper.Combine(Model, HashHelper.HashList(Materials));
}
}