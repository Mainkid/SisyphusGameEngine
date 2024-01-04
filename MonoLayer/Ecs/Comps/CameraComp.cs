using System;
using SyEngine.Editor.Attributes;
using SyEngine.Helpers;

namespace SyEngine.Ecs.Comps
{
public struct CameraComp : SyEcs.IComp
{
	public float NearPlane;
	public float FarPlane;
	public float FovDegrees;

	[Hide, NonSerialized]
	internal int? Hash;


	public override int GetHashCode()
		=> HashHelper.Combine(NearPlane, FarPlane, FovDegrees);
}
}