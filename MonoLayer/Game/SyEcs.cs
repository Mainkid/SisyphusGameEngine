using System;
using System.Collections.Generic;
using Leopotam.EcsLite;
using SyEngine.Game.Comps;
using SyEngine.Game.Datas;
using SyEngine.Logs;

namespace SyEngine.Game
{
public class SyEcs
{
    private readonly SyGameContext _context;
    private readonly EcsWorld              _world;


    internal SyEcs(SyGameContext internalContext)
    {
        _context = internalContext;

        _world            = new EcsWorld();
        _context.EcsWorld = _world;

        _singletonEntity = _world.NewEntity();
        _world.GetPool<SingletonHolderComp>().Add(_singletonEntity);
    }

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    private List<SyEcsSystemBase> _systems = new List<SyEcsSystemBase>();
    private int                   _singletonEntity;
    
    internal void SetSystems(List<SyEcsSystemBase> systems)
    {
        foreach (var system in systems)
            system.Attach(this);

        _systems = systems;
    }

    internal void InitSystems()
    {
        foreach (var system in _systems)
            system.Init();
    }

    internal void RunSystems()
    {
        foreach (var system in _systems)
            system.Run();
    }

    internal void DestroySystems()
    {
        foreach (var system in _systems)
            system.Destroy();
    }

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    public int CreateEntity()
    {
        return _world.NewEntity();
    }

    public void DestroyEntity(int ent)
    {
        _world.DelEntity(ent);
        TryDestroyEngineEntity(ent);
    }
    

    public ref T AddComp<T>(int ent) where T : struct, IComp
    {
        return ref _world.GetPool<T>().Add(ent);
    }

    public void RemoveComp<T>(int ent) where T : struct, IComp
    {
        _world.GetPool<T>().Del(ent);
    }
    
    
    public ref T GetComp<T>(int ent) where T : struct
    {
        return ref _world.GetPool<T>().Get(ent);
    }
    

    public ref TransformComp AddTransformComp(int ent)
    {
        SyLog.Debug(ELogTag.Ecs, $"add transform comp to g{ent}");
        
        TryCreateEngineEntity(ent);
        
        ref var comp = ref _world.GetPool<TransformComp>().Add(ent);
        comp.Scale = SyVector3.One;
        
        SyProxyGame.GE_AddTransformComp(_context.GameEntToEngineEnt[ent]);
        IncreaseEntityEngineCompsCount(ent);

        return ref comp;
    }

    public void RemoveTransformComp(int ent)
    {
        SyLog.Debug(ELogTag.Ecs, $"remove transform comp from g{ent}");
        
        _world.GetPool<TransformComp>().Del(ent);
        SyProxyGame.GE_RemoveTransformComp(_context.GameEntToEngineEnt[ent]);
        DecreaseEntityEngineCompsCount(ent);
    }

    public ref MeshComp AddMeshComp(int ent)
    {
        SyLog.Debug(ELogTag.Ecs, $"add mesh comp to g{ent}");
        
        TryCreateEngineEntity(ent);
        
        ref var comp = ref _world.GetPool<MeshComp>().Add(ent);
        
        SyProxyGame.GE_AddMeshComp(_context.GameEntToEngineEnt[ent]);
        IncreaseEntityEngineCompsCount(ent);

        return ref comp;
    }

    public void RemoveMeshComp(int ent)
    {
        SyLog.Debug(ELogTag.Ecs, $"remove mesh comp from g{ent}");

        TryCreateEngineEntity(ent);

        _world.GetPool<MeshComp>().Add(ent);
        SyProxyGame.GE_RemoveMeshComp(_context.GameEntToEngineEnt[ent]);
        DecreaseEntityEngineCompsCount(ent);
    }



    //-----------------------------------------------------------
    //-----------------------------------------------------------
    public EcsWorld.Mask BuildFilter<T>() where T : struct
    {
        return _world.Filter<T>();
    }

    public ref T AddSingleton<T>() where T : struct, IComp => ref AddSingletonInternal<T>();

    public ref T GetSingleton<T>() where T : struct
    {
        return ref _world.GetPool<T>().Get(_singletonEntity);
    }
    
    public void RemoveSingleton<T>() where T: struct, IComp => RemoveSingletonInternal<T>();

    internal ref T AddSingletonInternal<T>() where T: struct
    {
        return ref _world.GetPool<T>().Add(_singletonEntity);
    }

    internal void RemoveSingletonInternal<T>() where T: struct
    {
        _world.GetPool<T>().Del(_singletonEntity);
    }
    

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    private void TryCreateEngineEntity(int gameEnt)
    {
        if (_context.GameEntToEngineEnt.TryGetValue(gameEnt, out _))
            return;

        SyLog.Debug(ELogTag.Ecs, "create engine entity");
        uint engineEnt = SyProxyGame.GE_CreateEngineEntity();

        _context.GameEntToEngineEnt[gameEnt]        = engineEnt;
        _context.EngineEntToGameEnt[engineEnt]      = gameEnt;
        _context.GameEntToEngineCompsCount[gameEnt] = 0;
        SyLog.Debug(ELogTag.Ecs, $"entities pair created g{gameEnt} : e{engineEnt}");
    }
    
    private void TryDestroyEngineEntity(int gameEnt)
    {
        if (!_context.GameEntToEngineEnt.TryGetValue(gameEnt, out uint engineEnt))
            return;
        
        SyLog.Debug(ELogTag.Ecs, "destroy engine entity");
        SyProxyGame.GE_DestroyEngineEntity(engineEnt);

        _context.GameEntToEngineEnt.Remove(gameEnt);
        _context.EngineEntToGameEnt.Remove(engineEnt);
        _context.GameEntToEngineCompsCount.Remove(gameEnt);
    }

    private void IncreaseEntityEngineCompsCount(int gameEnt)
    {
        _context.GameEntToEngineCompsCount[gameEnt] += 1;
    }

    private void DecreaseEntityEngineCompsCount(int gameEnt)
    {
        _context.GameEntToEngineCompsCount[gameEnt] -= 1;
        if (_context.GameEntToEngineCompsCount[gameEnt] == 0)
            TryDestroyEngineEntity(gameEnt);
    }

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    public interface IComp { }
}
}
