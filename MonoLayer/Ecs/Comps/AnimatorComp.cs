using SyEngine.Helpers;
using SyEngine.Resources;

namespace SyEngine.Ecs.Comps
{
public struct AnimatorComp : SyEcs.IComp
{
	public ResRef<ResAnimation> Animation;

	public bool IsLooping;


	internal int? Hash;

	public override int GetHashCode()
		=> HashHelper.Combine(Animation, IsLooping);
}
}