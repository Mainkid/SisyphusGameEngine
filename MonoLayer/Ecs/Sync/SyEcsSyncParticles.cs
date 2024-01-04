using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using SyEngine.Datas;
using SyEngine.Ecs.Comps;
using SyEngine.Resources;

namespace SyEngine.Ecs.Sync
{
internal class SyEcsSyncParticles : SyEcsSyncBase<ParticlesComp, ProxyParticlesComp>
{
	public SyEcsSyncParticles(SyEcs ecs) : base(ecs) { }
	
	public override EEngineCompId Id => EEngineCompId.Particles;

	protected override void SendImpl(uint engineEnt, ref ParticlesComp particles)
	{
		var proxy = new ProxyParticlesComp();

		proxy.Duration             = particles.Duration;
		proxy.IsLooping            = particles.IsLooping;
		proxy.StartDelayTime       = particles.StartDelayTime;
		proxy.StartLifeTime        = particles.StartLifeTime;
		proxy.StartSpeed           = particles.StartSpeed;
		proxy.StartSize            = particles.StartSize;
		proxy.StartColor           = particles.StartColor;
		proxy.StartRotation        = particles.StartRotation;
		proxy.SizeOverLifetime     = particles.SizeOverLifetime;
		proxy.SpeedOverLifetime    = particles.SpeedOverLifetime;
		proxy.RotationOverLifetime = particles.RotationOverLifetime;
		proxy.MaxParticles         = particles.MaxParticles < 0 ? 0 : (uint)particles.MaxParticles;
		proxy.IsLit                = particles.IsLit;
		proxy.AmbientAmount        = particles.AmbientAmount;

		proxy.RateOverTime = particles.RateOverTime;

		if (particles.Bursts == null)
		{
			proxy.BurstsCount = 0;
			proxy.Bursts      = IntPtr.Zero;
		}
		else
		{
			proxy.BurstsCount = particles.Bursts.Count;
			int burstSize = Marshal.SizeOf<ParticlesComp.BurstData>();
			proxy.Bursts = Marshal.AllocHGlobal(burstSize * proxy.BurstsCount);
			var ptr = proxy.Bursts;
			for (var i = 0; i < proxy.BurstsCount; i++)
			{
				Marshal.StructureToPtr(particles.Bursts[i], ptr, true);
				ptr += burstSize;
			}
		}

		proxy.EmitShape = particles.EmitShape;
		proxy.Angle     = particles.Angle;
		proxy.Radius    = particles.Radius;

		proxy.TextureUuid = particles.Texture?.Uuid;

		SyProxyEcs.GeUpdateParticlesComp(engineEnt, proxy);

		if (proxy.Bursts != IntPtr.Zero)
			Marshal.FreeHGlobal(proxy.Bursts);
	}

	protected override void ReceiveImpl(ref ProxyParticlesComp proxy, ref ParticlesComp particles)
	{
		particles.Duration             = proxy.Duration;
		particles.IsLooping            = proxy.IsLooping;
		particles.StartDelayTime       = proxy.StartDelayTime;
		particles.StartLifeTime        = proxy.StartLifeTime;
		particles.StartSpeed           = proxy.StartSpeed;
		particles.StartSize            = proxy.StartSize;
		particles.StartColor           = proxy.StartColor;
		particles.StartRotation        = proxy.StartRotation;
		particles.SizeOverLifetime     = proxy.SizeOverLifetime;
		particles.SpeedOverLifetime    = proxy.SpeedOverLifetime;
		particles.RotationOverLifetime = proxy.RotationOverLifetime;
		particles.MaxParticles         = (int)proxy.MaxParticles;
		particles.IsLit                = proxy.IsLit;
		particles.AmbientAmount        = proxy.AmbientAmount;

		particles.RateOverTime = proxy.RateOverTime;

		if (particles.Bursts == null)
			particles.Bursts = new List<ParticlesComp.BurstData>();
		else
			particles.Bursts.Clear();
		int burstSize = Marshal.SizeOf<ParticlesComp.BurstData>();
		for (var i = 0; i < proxy.BurstsCount; i++)
		{
			var ptr = proxy.Bursts + i * burstSize;
			particles.Bursts.Add(Marshal.PtrToStructure<ParticlesComp.BurstData>(ptr));
		}

		particles.EmitShape = proxy.EmitShape;
		particles.Angle     = proxy.Angle;
		particles.Radius    = proxy.Radius;

		if (particles.Texture?.Uuid != proxy.TextureUuid)
			particles.Texture = proxy.TextureUuid == null ? null : new ResRef<ResTexture>(proxy.TextureUuid);
	}

	protected override int? GetHashImpl(ref ParticlesComp comp)
		=> 0;

	protected override void SetHashImpl(ref ParticlesComp comp, int hash)
		=> comp.IsDirty = false;
}

internal struct ProxyParticlesComp
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