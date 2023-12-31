﻿using System;
using SyEngine.Datas;
using SyEngine.Editor.Attributes;
using SyEngine.Helpers;

namespace SyEngine.Ecs.Comps
{
public struct RigidComp : SyEcs.IComp
{
	public EType Type;
	public float Mass;

	public bool IsAutoMass;
	public bool IsKinematic;
	public bool IsGravityOn;

	public SyVector3 LinearVelocity;
	public SyVector3 AngularVelocity;


	[Hide, NonSerialized]
	internal int? Hash;
	
	
	public override int GetHashCode()
		=> HashHelper.Combine(
			Type,
			Mass,
			IsAutoMass,
			IsKinematic,
			IsGravityOn,
			LinearVelocity,
			AngularVelocity
		);


	public enum EType
	{
		Static,
		Dynamic
	}
}
}