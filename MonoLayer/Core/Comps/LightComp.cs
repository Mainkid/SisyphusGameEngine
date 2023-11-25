using SyEngine.Core.Datas;
using SyEngine.Core.Helpers;

namespace SyEngine.Core.Comps
{
public struct LightComp : SyEcs.IComp
{
	public EType      Type;
	public EBehaviour Behaviour;
	public SyColor    Color;
	public float      PointLightRadius;
	public bool       ShouldCastShadows;

	internal int? Hash;

	public override int GetHashCode()
		=> HashHelper.Combine(Type, Behaviour, Color, PointLightRadius, ShouldCastShadows);

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