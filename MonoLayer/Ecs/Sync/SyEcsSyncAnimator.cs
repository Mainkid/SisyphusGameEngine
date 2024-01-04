using SyEngine.Ecs.Comps;
using SyEngine.Resources;

namespace SyEngine.Ecs.Sync
{
internal class SyEcsSyncAnimator : SyEcsSyncBase<AnimatorComp, ProxyAnimatorComp>
{
	public SyEcsSyncAnimator(SyEcs ecs) : base(ecs) { }

	public override EEngineCompId Id => EEngineCompId.Animator;

	protected override void SendImpl(uint engineEnt, ref AnimatorComp comp)
	{
		var proxy = new ProxyAnimatorComp
		{
			AnimationUuid = comp.Animation?.Uuid,
			IsLooping     = comp.IsLooping
		};
		SyProxyEcs.GeUpdateAnimatorComp(engineEnt, proxy);
	}

	protected override void ReceiveImpl(ref ProxyAnimatorComp proxy, ref AnimatorComp comp)
	{
		comp.Animation = proxy.AnimationUuid == null ? null : new ResRef<ResAnimation>(proxy.AnimationUuid);
		comp.IsLooping = proxy.IsLooping;
	}

	protected override int? GetHashImpl(ref AnimatorComp comp)
		=> comp.Hash;

	protected override void SetHashImpl(ref AnimatorComp comp, int hash)
		=> comp.Hash = hash;
}

internal struct ProxyAnimatorComp
{
	public string AnimationUuid;
	public bool   IsLooping;
}
}