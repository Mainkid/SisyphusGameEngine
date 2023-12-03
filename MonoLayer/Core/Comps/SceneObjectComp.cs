using SyEngine.Core.Helpers;

namespace SyEngine.Core.Comps
{
public struct SceneObjectComp : SyEcs.IInternalComp
{
	public string Name;
	public bool   IsActive;

	internal int Hash;

	public override int GetHashCode()
		=> HashHelper.Combine(Name, IsActive);
}
}