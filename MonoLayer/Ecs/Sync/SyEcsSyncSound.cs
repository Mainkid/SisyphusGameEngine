using SyEngine.Ecs.Comps;
using SyEngine.Resources;

namespace SyEngine.Ecs.Sync
{
internal class SyEcsSyncSound : SyEcsSyncBase<SoundComp, ProxySoundComp>
{
	public SyEcsSyncSound(SyEcs ecs) : base(ecs) { }
	
	public override EEngineCompId Id => EEngineCompId.Sound;

	protected override void SendImpl(uint engineEnt, ref SoundComp sound)
	{
		var proxy = new ProxySoundComp
		{
			IsPlaying = sound.IsPlaying,
			IsLooping = sound.IsLooping,
			Is3d      = sound.Is3d,
			Volume    = sound.Volume,
			SoundUuid = sound.Sound?.Uuid
		};

		SyProxyEcs.GeUpdateSoundComp(engineEnt, proxy);
	}

	protected override void ReceiveImpl(ref ProxySoundComp proxy, ref SoundComp sound)
	{
		sound.IsPlaying = proxy.IsPlaying;
		sound.IsLooping = proxy.IsLooping;
		sound.Is3d      = proxy.Is3d;
		sound.Volume    = proxy.Volume;
		sound.Sound     = proxy.SoundUuid == null ? null : new ResRef<ResSound>(proxy.SoundUuid);
	}

	protected override int? GetHashImpl(ref SoundComp comp)
		=> comp.Hash;

	protected override void SetHashImpl(ref SoundComp comp, int hash)
		=> comp.Hash = hash;
}

internal struct ProxySoundComp
{
	public bool  IsPlaying;
	public bool  IsLooping;
	public float Volume;
	public bool  Is3d;

	public string SoundUuid;
}
}