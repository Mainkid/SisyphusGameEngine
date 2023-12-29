using System.Collections.Generic;
using SyEngine.Core.Datas;
using SyEngine.Core.Helpers;

namespace SyEngine.Core.Comps
{
public struct ParticlesComp : SyEcs.IComp
{
	public float              Duration;                                   
	public bool               IsLooping;                                   
	public ParticlesInputData StartDelayTime;                
	//public ParticlesInputData StartLifeTime;                 
	//public ParticlesInputData StartSpeed;                    
	//public ParticlesInputData StartSize;                     
	//public ParticlesInputData StartColor;                 
	//public ParticlesInputData StartRotation;				
	//public ParticlesInputData SizeOverLifetime;					 
	//public ParticlesInputData SpeedOverLifetime;
	//public ParticlesInputData RotationOverLifetime;
	//public uint               MaxParticles;
	//public bool               IsLit;
	//public float              AmbientAmount;
	//
	//public ParticlesInputData RateOverTime;
	//
	//public EEmitShape ParticleEmitShape;
	//public float      Angle;
	//public float      Radius;


	public bool IsDirty;
	

	public enum EEmitShape
	{
		Sphere,
		Cone
	}
}

public struct ParticlesInputData
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

public struct ParticlesBurstData
{
	public float              Time;
	public float              TimeSinceLastBurst;
	public ParticlesInputData Count;
	public bool               IsLooping;
	public float              Probability;
}
}