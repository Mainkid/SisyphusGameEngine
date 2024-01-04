using System;
using System.Collections.Generic;
using SyEngine.Datas;
using SyEngine.Editor.Attributes;
using SyEngine.Helpers;
using SyEngine.Resources;

namespace SyEngine.Ecs.Comps
{
public struct ParticlesComp : SyEcs.IComp
{
	public float Duration;
	public bool  IsLooping;
	[Foldout]
	public InputFloat StartDelayTime;
	[Foldout]
	public InputFloat StartLifeTime;
	[Foldout]
	public InputFloat StartSpeed;
	[Foldout]
	public InputFloat StartSize;
	[Foldout]
	public InputColor StartColor;
	[Foldout]
	public InputFloat StartRotation;
	[Foldout]
	public SyCurve SizeOverLifetime;
	[Foldout]
	public SyCurve SpeedOverLifetime;
	[Foldout]
	public InputFloat RotationOverLifetime;

	public bool  IsLit;
	public float AmbientAmount;

	public int MaxParticles;
	[Foldout]
	public InputFloat RateOverTime;
	public List<BurstData> Bursts;

	public EEmitShape EmitShape;
	public float      Angle;
	public float      Radius;

	public ResRef<ResTexture> Texture;

	
	[Hide, NonSerialized]
	public bool IsDirty;


	public override int GetHashCode() => IsDirty ? 1 : 0;


	public struct InputFloat
	{
		public bool IsRange;
		[ShowIfBool(nameof(IsRange), false)]
		public float Value;
		[ShowIfBool(nameof(IsRange), true)]
		public float From;
		[ShowIfBool(nameof(IsRange), true)]
		public float To;

		public override int GetHashCode()
			=> HashHelper.Combine(IsRange, Value, From, To);
	}

	public struct InputColor
	{
		public bool IsRange;
		[ShowIfBool(nameof(IsRange), false)]
		public SyColor Value;
		[ShowIfBool(nameof(IsRange), true)]
		public SyColor From;
		[ShowIfBool(nameof(IsRange), true)]
		public SyColor To;

		public override int GetHashCode()
			=> HashHelper.Combine(IsRange, Value, From, To);
	}

	public struct BurstData
	{
		public float      Time;
		public float      TimeSinceLastBurst;
		public InputFloat Count;
		public bool       IsLooping;
		public float      Probability;

		public override int GetHashCode()
			=> HashHelper.Combine(Time, TimeSinceLastBurst, Count, IsLooping, Probability);
	}

	public enum EEmitShape
	{
		Sphere,
		Cone
	}
}
}