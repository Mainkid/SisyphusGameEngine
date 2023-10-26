using System;
using System.Collections.Generic;
using Leopotam.EcsLite;
using SyEngine.Game.Comps;
using SyEngine.Game.Datas;

namespace SyEngine.Game
{
public class SyEcs
{
    private readonly SyGameInternalContext _context;
    private readonly EcsWorld              _world;


    internal SyEcs(SyGameInternalContext internalContext)
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
    

    public void AddTransformComp(int ent)
    {
        Console.WriteLine($"[game] add transform comp to g{ent}");
        
        TryCreateEngineEntity(ent);
        
        _world.GetPool<TransformComp>().Add(ent).Scale = SyVector3.One;
        SyProxyGame.GE_AddTransformComp(_context.GameEntToEngineEnt[ent]);
        IncreaseEntityEngineCompsCount(ent);
    }

    public void RemoveTransformComp(int ent)
    {
        Console.WriteLine($"[game] remove transform comp from f{ent}");
        
        _world.GetPool<TransformComp>().Del(ent);
        SyProxyGame.GE_RemoveTransformComp(_context.GameEntToEngineEnt[ent]);
        DecreaseEntityEngineCompsCount(ent);
    }

    public void AddMeshComp(int ent)
    {
        Console.WriteLine($"[game] add mesh comp to g{ent}");
        
        TryCreateEngineEntity(ent);
        
        _world.GetPool<MeshComp>().Add(ent);
        SyProxyGame.GE_AddMeshComp(_context.GameEntToEngineEnt[ent]);
        IncreaseEntityEngineCompsCount(ent);
    }

    public void RemoveMeshComp(int ent)
    {
        Console.WriteLine($"[game] remove mesh comp from g{ent}");

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

        Console.WriteLine("[game] create engine entity");
        uint engineEnt = SyProxyGame.GE_CreateEngineEntity();

        _context.GameEntToEngineEnt[gameEnt]        = engineEnt;
        _context.EngineEntToGameEnt[engineEnt]      = gameEnt;
        _context.GameEntToEngineCompsCount[gameEnt] = 0;
        Console.WriteLine($"[game] entities pair created g{gameEnt} : e{engineEnt}");
    }
    
    private void TryDestroyEngineEntity(int gameEnt)
    {
        if (!_context.GameEntToEngineEnt.TryGetValue(gameEnt, out uint engineEnt))
            return;
        
        Console.WriteLine("[game] destroy engine entity");
        SyProxyGame.GE_DestroyEngineEntity();

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
