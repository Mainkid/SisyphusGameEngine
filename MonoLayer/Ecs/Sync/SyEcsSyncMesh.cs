using System.Collections.Generic;
using SyEngine.Ecs.Comps;
using SyEngine.Resources;

namespace SyEngine.Ecs.Sync
{
internal class SyEcsSyncMesh : SyEcsSyncBase<MeshComp, ProxyMeshComp>
{
	public SyEcsSyncMesh(SyEcs ecs) : base(ecs) { }

	public override EEngineCompId Id => EEngineCompId.Mesh;

	protected override void SendImpl(uint engineEnt, ref MeshComp mesh)
	{
		var proxy = new ProxyMeshComp();
		proxy.ModelUuid = mesh.Model?.Uuid;
		if (mesh.Materials == null || mesh.Materials.Count == 0)
		{
			proxy.MaterialsUuids = null;
		}
		else
		{
			proxy.MaterialsUuids = new string[mesh.Materials.Count];
			for (var i = 0; i < mesh.Materials.Count; i++)
				proxy.MaterialsUuids[i] = mesh.Materials[i]?.Uuid;
		}
            
		SyProxyEcs.GeUpdateMeshComp(engineEnt, proxy);
	}

	protected override void ReceiveImpl(ref ProxyMeshComp proxy, ref MeshComp mesh)
	{
		mesh.Model = proxy.ModelUuid == null ? null : new ResRef<ResModel>(proxy.ModelUuid);

		if (mesh.Materials == null)
			mesh.Materials = new List<ResRef<ResMaterial>>();
		mesh.Materials.Clear();
        
		if (proxy.MaterialsUuids != null)
			foreach (string uuid in proxy.MaterialsUuids)
				mesh.Materials.Add(new ResRef<ResMaterial>(uuid));
	}

	protected override int? GetHashImpl(ref MeshComp comp)
		=> comp.Hash;

	protected override void SetHashImpl(ref MeshComp comp, int hash)
		=> comp.Hash = hash;
}
    
internal struct ProxyMeshComp
{
	public string   ModelUuid;
	public string[] MaterialsUuids;
}
}