using SyEngine.Core.Comps;
using SyEngine.Core.Datas;

namespace SyEngine.Core.ProxyComps
{
public struct ProxyColliderComp
{
	public ColliderComp.EType Type;
	public SyVector3          Extent;
	public float              Radius;
	public float              HalfHeight;
}
}