using SyEngine.Datas;
using SyEngine.Ecs.Comps;

namespace SyEngine.Ecs.Sync
{
internal class SyEcsSyncLight : SyEcsSyncBase<LightComp, ProxyLightComp>
{
	public SyEcsSyncLight(SyEcs ecs) : base(ecs) { }
	
	public override EEngineCompId Id => EEngineCompId.Light;

	protected override void SendImpl(uint engineEnt, ref LightComp light)
	{
		var proxy = new ProxyLightComp
		{
			Type              = light.Type,
			Behaviour         = light.Behaviour,
			Color             = light.Color.WithA(light.Intensity),
			PointLightRadius  = light.PointLightRadius,
			ShouldCastShadows = light.ShouldCastShadows
		};
		SyProxyEcs.GeUpdateLightComp(engineEnt, proxy);
	}

	protected override void ReceiveImpl(ref ProxyLightComp proxy, ref LightComp light)
	{
		light.Type              = proxy.Type;
		light.Behaviour         = proxy.Behaviour;
		light.Color             = proxy.Color;
		light.Intensity         = proxy.Color.W;
		light.PointLightRadius  = proxy.PointLightRadius;
		light.ShouldCastShadows = proxy.ShouldCastShadows;
	}

	protected override int? GetHashImpl(ref LightComp comp)
		=> comp.Hash;

	protected override void SetHashImpl(ref LightComp comp, int hash)
		=> comp.Hash = hash;
}
    
internal struct ProxyLightComp
{
	public LightComp.EType      Type;
	public LightComp.EBehaviour Behaviour;
	public SyVector4            Color;
	public float                PointLightRadius;
	public bool                 ShouldCastShadows;
}
}