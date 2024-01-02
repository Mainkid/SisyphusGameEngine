using System;
using SyEngine.Core.Comps;
using SyEngine.Core.Datas;

namespace SyEngine.Core.ProxyComps
{
public struct ProxyParticlesComp : SyEcs.IComp
{
	public float                    Duration;
	public bool                     IsLooping;
	public ParticlesComp.InputFloat StartDelayTime;
	public ParticlesComp.InputFloat StartLifeTime;
	public ParticlesComp.InputFloat StartSpeed;
	public ParticlesComp.InputFloat StartSize;
	public ParticlesComp.InputColor StartColor;
	public ParticlesComp.InputFloat StartRotation;
	public SyCurve                  SizeOverLifetime;
	public SyCurve                  SpeedOverLifetime;
	public ParticlesComp.InputFloat RotationOverLifetime;
	public uint                     MaxParticles;
	public bool                     IsLit;
	public float                    AmbientAmount;

	public ParticlesComp.InputFloat RateOverTime;

	public IntPtr Bursts;
	public int    BurstsCount;

	public ParticlesComp.EEmitShape EmitShape;
	public float                    Angle;
	public float                    Radius;

	public string TextureUuid;
}
}