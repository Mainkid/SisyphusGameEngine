using Leopotam.EcsLite;
using SyEngine.Game.Comps;

namespace SyEngine.Game
{
internal class SyEcsSync
{
    private readonly SyEcs                 _ecs;
    private readonly SyGameInternalContext _context;

    private readonly EcsPool<TransformComp> _transformsPool;
    private readonly EcsPool<MeshComp>      _meshesPool;

    private readonly EcsFilter _transformsFilter;
    private readonly EcsFilter _meshesFilter;

    public SyEcsSync(SyEcs ecs, SyGameInternalContext context)
    {
        _ecs     = ecs;
        _context = context;

        _transformsPool = context.EcsWorld.GetPool<TransformComp>();
        _meshesPool     = context.EcsWorld.GetPool<MeshComp>();

        _transformsFilter = context.EcsWorld.Filter<TransformComp>().End();
        _meshesFilter     = context.EcsWorld.Filter<MeshComp>().End();
    }

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    public void SendTransformsToEngine()
    {
        foreach (int ent in _transformsFilter)
        {
            ref var tf = ref _transformsPool.Get(ent);
            SyProxyGame.GE_UpdateTransformComp(_context.GameEntToEngineEnt[ent], tf);
        }
    }

    public void ReceiveTransformFromEngine(uint engineEnt, TransformComp comp)
    {
        ref var tf = ref _transformsPool.Get(_context.EngineEntToGameEnt[engineEnt]);
        tf = comp;
    }

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    public void SendMeshesToEngine()
    {
        foreach (int ent in _meshesFilter)
        {
            ref var mesh = ref _meshesPool.Get(ent);
            SyProxyGame.GE_UpdateMeshComp(_context.GameEntToEngineEnt[ent], mesh);
        }
    }

    public void ReceiveMeshFromEngine(uint engineEnt, MeshComp comp)
    {
        ref var mesh = ref _meshesPool.Get(_context.EngineEntToGameEnt[engineEnt]);
        mesh = comp;
    }
}
}
