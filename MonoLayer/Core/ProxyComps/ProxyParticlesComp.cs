using System;
using System.Runtime.InteropServices;
using SyEngine.Core.Comps;

namespace SyEngine.Core.ProxyComps
{
public struct ProxyParticlesComp : SyEcs.IComp
{
	public float                   Duration;
	public bool                    IsLooping;
	public ParticlesComp.InputData StartDelayTime;
	public ParticlesComp.InputData StartLifeTime;
	public ParticlesComp.InputData StartSpeed;
	public ParticlesComp.InputData StartSize;
	public ParticlesComp.InputData StartColor;
	public ParticlesComp.InputData StartRotation;
	public ParticlesComp.InputData SizeOverLifetime;
	public ParticlesComp.InputData SpeedOverLifetime;
	public ParticlesComp.InputData RotationOverLifetime;
	public uint                    MaxParticles;
	public bool                    IsLit;
	public float                   AmbientAmount;

	public ParticlesComp.InputData RateOverTime;

	public IntPtr Bursts;
	public int    BurstsCount;

	public ParticlesComp.EEmitShape EmitShape;
	public float                    Angle;
	public float                    Radius;

	public string TextureUuid;
}
}