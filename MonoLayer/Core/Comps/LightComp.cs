using SyEngine.Core.Datas;

namespace SyEngine.Core.Comps
{
public struct LightComp : SyEcs.IComp
{
	public EType      Type;
	public EBehaviour Behaviour;
	public SyColor    Color;
	public bool       ShouldCastShadows;
	public SyVector4  ParamsRadiusAndAttenuation;
	
	
	public enum EType
	{
		Ambient,
		Directional,
		PointLight,
		SpotLight
	}

	public enum EBehaviour
	{
		Static,
		Movable
	}
}
}