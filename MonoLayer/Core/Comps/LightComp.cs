using Leopotam.EcsLite;
using SyEngine.Core.Datas;
using SyEngine.Core.Helpers;
using SyEngine.Editor.Attributes;

namespace SyEngine.Core.Comps
{
public struct LightComp : SyEcs.IComp
{
	public EType      Type;
	public EBehaviour Behaviour;
	public SyColor    Color;
    
	[ShowIfEnum(nameof(Type), (int)EType.PointLight)]
	[FloatRange(min:0.0f)]
	public float PointLightRadius;
	[ShowIfEnum(nameof(Type), (int)EType.PointLight)]
	public bool  ShouldCastShadows;

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