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

    private readonly EcsFilter _transformsFilter;
    private readonly EcsFilter _meshesFilter;

    public SyEcsSync(SyEcs ecs)
    {
        _ecs = ecs;

        _transformsPool = ecs.World.GetPool<TransformComp>();
        _meshesPool     = ecs.World.GetPool<MeshComp>();

        _transformsFilter = ecs.World.Filter<TransformComp>().End();
        _meshesFilter     = ecs.World.Filter<MeshComp>().End();
    }

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    public void SyncEngineWithGame()
    {
        SendTransformsToEngine();
        //SendMeshesToEngine();
    }

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
}
}
