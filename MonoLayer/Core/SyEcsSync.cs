using Leopotam.EcsLite;
using SyEngine.Core.Comps;
using SyEngine.Core.ProxyComps;

namespace SyEngine.Core
{
internal class SyEcsSync
{
    private readonly SyEcs _ecs;

    private readonly EcsPool<TransformComp> _transformsPool;
    private readonly EcsPool<MeshComp>      _meshesPool;
    private readonly EcsPool<LightComp>     _lightsPool;
    private readonly EcsPool<RigidComp>     _rigidsPool;

    private readonly EcsFilter _transformsFilter;
    private readonly EcsFilter _meshesFilter;
    private readonly EcsFilter _lightsFilter;
    private readonly EcsFilter _rigidsFilter;

    public SyEcsSync(SyEcs ecs)
    {
        _ecs = ecs;

        _transformsPool = ecs.World.GetPool<TransformComp>();
        _meshesPool     = ecs.World.GetPool<MeshComp>();
        _lightsPool     = ecs.World.GetPool<LightComp>();
        _rigidsPool     = ecs.World.GetPool<RigidComp>();

        _transformsFilter = ecs.World.Filter<TransformComp>().End();
        _meshesFilter     = ecs.World.Filter<MeshComp>().End();
        _lightsFilter     = ecs.World.Filter<LightComp>().End();
        _rigidsFilter     = ecs.World.Filter<RigidComp>().End();
    }

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    public void SyncEngineWithGame()
    {
        SendTransformsToEngine();
        //SendMeshesToEngine();
    }

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    private void SendTransformsToEngine()
    {
        foreach (int ent in _transformsFilter)
        {
            ref var tf = ref _transformsPool.Get(ent);

            uint engineParentEnt = default;
            bool hasParent = tf.ParentEnt != null &&
                             _ecs.ToEngineEnt(tf.ParentEnt.Value, out engineParentEnt);

            var proxy = new ProxyTransformComp
            {
                LocalPosition   = tf.LocalPosition,
                LocalRotation   = tf.LocalRotation,
                LocalScale      = tf.LocalScale,
                HasParent       = hasParent,
                ParentEngineEnt = engineParentEnt
            };

            SyProxyEcs.GeUpdateTransformComp(_ecs.ToEngineEnt(ent), proxy);
        }
    }

    public void ReceiveTransformFromEngine(uint engineEnt, ProxyTransformComp proxy)
    {
        ref var tf = ref _transformsPool.Get(_ecs.ToGameEnt(engineEnt));

        tf.Position      = proxy.Position;
        tf.Rotation      = proxy.Rotation;
        tf.Scale         = proxy.Scale;
        tf.LocalPosition = proxy.LocalPosition;
        tf.LocalRotation = proxy.LocalRotation;
        tf.LocalScale    = proxy.LocalScale;

        tf.ParentEnt = proxy.HasParent &&
                       _ecs.ToGameEnt(proxy.ParentEngineEnt, out int parentEnt)
            ? (int?)parentEnt
            : null;
    }

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    private void SendMeshesToEngine()
    {
        foreach (int ent in _meshesFilter)
        {
            ref var mesh = ref _meshesPool.Get(ent);
            SyProxyEcs.GeUpdateMeshComp(_ecs.ToEngineEnt(ent), mesh);
        }
    }

    public void ReceiveMeshFromEngine(uint engineEnt, MeshComp comp)
    {
        ref var mesh = ref _meshesPool.Get(_ecs.ToGameEnt(engineEnt));
        mesh = comp;
    }

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    private void SendLightsToEngine()
    {
        foreach (int ent in _lightsFilter)
        {
            ref var light = ref _lightsPool.Get(ent);
            SyProxyEcs.GeUpdateLightComp(_ecs.ToEngineEnt(ent), light);
        }
    }

    public void ReceiveLightFromEngine(uint engineEnt, LightComp comp)
    {
        ref var light = ref _lightsPool.Get(_ecs.ToGameEnt(engineEnt));
        light = comp;
    }

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    private void SendRigidsToEngine()
    {
        foreach (int ent in _rigidsFilter)
        {
            ref var rigid = ref _rigidsPool.Get(ent);
            SyProxyEcs.GeUpdateRigidComp(_ecs.ToEngineEnt(ent), rigid);
        }
    }

    public void ReceiveRigidFromEngine(uint engineEnt, RigidComp comp)
    {
        ref var rigid = ref _rigidsPool.Get(_ecs.ToGameEnt(engineEnt));
        rigid = comp;
    }
}
}
