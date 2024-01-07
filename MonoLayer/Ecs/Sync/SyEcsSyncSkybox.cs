using SyEngine.Ecs.Comps;
using SyEngine.Resources;

namespace SyEngine.Ecs.Sync
{
internal class SyEcsSyncSkybox : SyEcsSyncBase<SkyboxComp, ProxySkyboxComp>
{
	public SyEcsSyncSkybox(SyEcs ecs) : base(ecs) { }

	public override EEngineCompId Id => EEngineCompId.Skybox;

	protected override void SendImpl(uint engineEnt, ref SkyboxComp skybox)
	{
		var proxy = new ProxySkyboxComp
		{
			CubemapUuid = skybox.Cubemap?.Uuid
		};
		SyProxyEcs.GeUpdateSkyboxComp(engineEnt, proxy);
	}

	protected override void ReceiveImpl(ref ProxySkyboxComp proxy, ref SkyboxComp skybox)
	{
		skybox.Cubemap = proxy.CubemapUuid == null ? null : new ResRef<ResCubemap>(proxy.CubemapUuid);
	}

	protected override int? GetHashImpl(ref SkyboxComp comp)
		=> comp.Hash;

	protected override void SetHashImpl(ref SkyboxComp comp, int hash)
		=> comp.Hash = hash;
}

internal struct ProxySkyboxComp
{
	public string CubemapUuid;
}
}