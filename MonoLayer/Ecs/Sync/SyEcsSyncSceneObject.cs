using SyEngine.Ecs.Comps;

namespace SyEngine.Ecs.Sync
{
internal class SyEcsSyncSceneObject : SyEcsSyncBase<SceneObjectComp, ProxySceneObjectComp>
{
	public SyEcsSyncSceneObject(SyEcs ecs) : base(ecs) { }

	public override EEngineCompId Id => EEngineCompId.SceneObject;

	protected override void SendImpl(uint engineEnt, ref SceneObjectComp comp)
	{
		var proxy = new ProxySceneObjectComp
		{
			Name     = comp.Name,
			IsActive = comp.IsActive
		};
		SyProxyEcs.GeUpdateSceneObjectComp(engineEnt, proxy);
	}

	protected override void ReceiveImpl(ref ProxySceneObjectComp proxy, ref SceneObjectComp comp)
	{
		comp.Name     = proxy.Name;
		comp.IsActive = proxy.IsActive;
	}

	protected override int? GetHashImpl(ref SceneObjectComp comp)
		=> comp.Hash;

	protected override void SetHashImpl(ref SceneObjectComp comp, int hash)
		=> comp.Hash = hash;
}
    
internal struct ProxySceneObjectComp
{
	public string Name;
	public bool   IsActive;
}
}