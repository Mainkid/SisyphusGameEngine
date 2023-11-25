using SyEngine.Core.Comps;
using SyEngine.Core.Datas;

namespace SyEngine.Core.ProxyComps
{
public struct ProxyLightComp
{
	public LightComp.EType      Type;
	public LightComp.EBehaviour Behaviour;
	public SyVector4            Color;
	public float                PointLightRadius;
	public bool                 ShouldCastShadows;
}
}