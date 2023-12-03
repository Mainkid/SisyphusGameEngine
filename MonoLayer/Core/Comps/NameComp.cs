using SyEngine.Core.Helpers;

namespace SyEngine.Core.Comps
{
public struct NameComp : SyEcs.IComp
{
	public string Name;

	internal string PrevName;
}
}