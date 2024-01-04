using System;
using SyEngine.Datas;
using SyEngine.Editor.Attributes;
using SyEngine.Helpers;

namespace SyEngine.Ecs.Comps
{
public struct LightComp : SyEcs.IComp
{
	public EType      Type;
	public EBehaviour Behaviour;
	[ColorNoAlpha]
	public SyColor Color;
	public float Intensity;
    
	[ShowIfEnum(nameof(Type), (int)EType.PointLight)]
	[FloatRange(min:0.0f)]
	public float PointLightRadius;
	[ShowIfEnum(nameof(Type), (int)EType.PointLight)]
	public bool  ShouldCastShadows;

	
	[Hide, NonSerialized]
	internal int? Hash;
	

	public override int GetHashCode()
		=> HashHelper.Combine(Type, Behaviour, Color, Intensity, PointLightRadius, ShouldCastShadows);
	
	
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