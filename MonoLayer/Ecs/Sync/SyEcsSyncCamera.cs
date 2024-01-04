using SyEngine.Ecs.Comps;

namespace SyEngine.Ecs.Sync
{
internal class SyEcsSyncCamera : SyEcsSyncBase<CameraComp, ProxyCameraComp>
{
	public SyEcsSyncCamera(SyEcs ecs) : base(ecs) { }

	public override EEngineCompId Id => EEngineCompId.Camera;

	protected override void SendImpl(uint engineEnt, ref CameraComp comp)
	{
		var proxy = new ProxyCameraComp
		{
			FarPlane   = comp.FarPlane,
			NearPlane  = comp.NearPlane,
			FovDegrees = comp.FovDegrees
		};
		SyProxyEcs.GeUpdateCameraComp(engineEnt, proxy);
	}

	protected override void ReceiveImpl(ref ProxyCameraComp proxy, ref CameraComp comp)
	{
		comp.NearPlane  = proxy.NearPlane;
		comp.FarPlane   = proxy.FarPlane;
		comp.FovDegrees = proxy.FovDegrees;
	}

	protected override int? GetHashImpl(ref CameraComp comp)
		=> comp.Hash;

	protected override void SetHashImpl(ref CameraComp comp, int hash)
		=> comp.Hash = hash;
}

public struct ProxyCameraComp
{
	public float NearPlane;
	public float FarPlane;
	public float FovDegrees;
}
}