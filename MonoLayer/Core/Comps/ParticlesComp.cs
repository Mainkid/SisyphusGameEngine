using System.Collections.Generic;
using Leopotam.EcsLite;
using SyEngine.Core.Datas;
using SyEngine.Core.Helpers;
using SyEngine.Core.Resources;

namespace SyEngine.Core.Comps
{
public struct ParticlesComp : SyEcs.IComp
{
	public float     Duration;
	public bool      IsLooping;
	public InputData StartDelayTime;
	public InputData StartLifeTime;
	public InputData StartSpeed;
	public InputData StartSize;
	public InputData StartColor;
	public InputData StartRotation;
	public InputData SizeOverLifetime;
	public InputData SpeedOverLifetime;
	public InputData RotationOverLifetime;

	public bool  IsLit;
	public float AmbientAmount;
	
	public int             MaxParticles;
	public InputData       RateOverTime;
	public List<BurstData> Bursts;

	public EEmitShape EmitShape;
	public float      Angle;
	public float      Radius;

	public ResRef<ResTexture> Texture;

	public bool IsDirty;
    

	public struct InputData
	{
		public float         F;
		public SyVector2     RandomBetweenConstsF;
		public SyCurve       Curve;
		public SyVector4     V;
		public SyVector4Pair RandomBetweenConstsV;
		public EInputType    InputType;


		public override int GetHashCode()
			=> HashHelper.Combine(
				F,
				RandomBetweenConstsF,
				Curve,
				V,
				RandomBetweenConstsV,
				InputType
			);


		public enum EInputType
		{
			SimpleFloat,
			RandomBetweenFloats,
			SimpleVector,
			RandomBetweenVectors,
			SimpleCurve
		}
	}

	public struct BurstData
	{
		public float     Time;
		public float     TimeSinceLastBurst;
		public InputData Count;
		public bool      IsLooping;
		public float     Probability;
	}

	public enum EEmitShape
	{
		Sphere,
		Cone
	}
}
}