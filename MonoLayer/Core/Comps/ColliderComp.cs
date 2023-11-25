using SyEngine.Core.Datas;
using SyEngine.Core.Helpers;

namespace SyEngine.Core.Comps
{
public struct ColliderComp : SyEcs.IComp
{
	public EType     Type;
	public SyVector3 Extent;
	public float     Radius;
	public float     HalfHeight;


	internal int? Hash;

	public override int GetHashCode()
		=> HashHelper.Combine(Type, Extent, Radius, HalfHeight);


	public enum EType
	{
		Box     = 0,
		Sphere  = 1,
		Capsule = 2
	}
}
}