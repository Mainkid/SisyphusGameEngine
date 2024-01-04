using System;
using System.Collections.Generic;

namespace SyEngine.Ecs.Sync
{
internal class SyEcsSyncer
{
    private readonly List<ISyEcsSync>             _all;
    private readonly Dictionary<Type, ISyEcsSync> _proxyToSync;
    
    public SyEcsSyncer(SyEcs ecs)
    {
        _all         = new List<ISyEcsSync>();
        _proxyToSync = new Dictionary<Type, ISyEcsSync>();
        
        Add(new SyEcsSyncSceneObject(ecs));
        Add(new SyEcsSyncTransform(ecs));
        Add(new SyEcsSyncMesh(ecs));
        Add(new SyEcsSyncLight(ecs));
        Add(new SyEcsSyncCollider(ecs));
        Add(new SyEcsSyncRigid(ecs));
        Add(new SyEcsSyncSkybox(ecs));
        Add(new SyEcsSyncParticles(ecs));
        Add(new SyEcsSyncSound(ecs));
        Add(new SyEcsSyncFixedJoint(ecs));
        Add(new SyEcsSyncHingeJoint(ecs));
        Add(new SyEcsSyncCamera(ecs));
    }

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    public void TrySendAll()
    {
        foreach (var sync in _all)
            sync.TrySendAll();
    }

    public void Receive<TProxy>(uint engineEnt, ref TProxy proxy)
    {
        var type = typeof(TProxy);
        var sync = (ISyEcsSync<TProxy>)_proxyToSync[type];
        sync.Receive(engineEnt, ref proxy);
    }


    private void Add<TSync>(TSync sync) where TSync: ISyEcsSync
    {
        _all.Add(sync);
        _proxyToSync.Add(sync.ProxyType, sync);
    }
}
}
