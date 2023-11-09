using System;
using System.Collections.Generic;
using Leopotam.EcsLite;
using SyEngine.Core.Comps;
using SyEngine.Core.Datas;
using SyEngine.Logger;

namespace SyEngine.Core
{
public class SyEcs
{
    internal readonly EcsWorld World;
    
    private readonly Dictionary<int, uint> _gameEntToEngineEnt        = new Dictionary<int, uint>();
    private readonly Dictionary<uint, int> _engineEntToGameEnt        = new Dictionary<uint, int>();
    private readonly Dictionary<int, int>  _gameEntToEngineCompsCount = new Dictionary<int, int>();
    
    internal SyEcs()
    {
        World = new EcsWorld();

        _singletonEntity = World.NewEntity();
        World.GetPool<SingletonsTag>().Add(_singletonEntity);
    }

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    private List<SyEcsSystemBase> _systems = new List<SyEcsSystemBase>();
    
    private int _singletonEntity;
    
    
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
        return World.NewEntity();
    }
    
    public void DestroyEntity(int ent)
    {
        World.DelEntity(ent);
        TryDestroyEngineEntity(ent);
    }

    internal void ContinueEntityDestroyCascade(uint engineEnt)
    {
        if (!_engineEntToGameEnt.TryGetValue(engineEnt, out int gameEnt))
            return;

        SyLog.Debug(ELogTag.Ecs, $"continue entity destroy cascade, g{gameEnt} destroy");
        World.DelEntity(gameEnt);

        _gameEntToEngineEnt.Remove(gameEnt);
        _engineEntToGameEnt.Remove(engineEnt);
        _gameEntToEngineCompsCount.Remove(gameEnt);
    }
    

    public ref T AddComp<T>(int ent) where T : struct, IComp
    {
        return ref World.GetPool<T>().Add(ent);
    }

    public void RemoveComp<T>(int ent) where T : struct, IComp
    {
        World.GetPool<T>().Del(ent);
    }
    
    
    public ref T GetComp<T>(int ent) where T : struct
    {
        return ref World.GetPool<T>().Get(ent);
    }
    

    public ref TransformComp AddTransformComp(int ent)
    {
        SyLog.Debug(ELogTag.Ecs, $"add transform comp to g{ent}");
        
        TryCreateEngineEntity(ent);
        
        ref var comp = ref World.GetPool<TransformComp>().Add(ent);
        comp.LocalScale = SyVector3.One;
        
        SyProxyEcs.GeAddTransformComp(_gameEntToEngineEnt[ent]);
        IncreaseEngineCompsCount(ent);

        return ref comp;
    }

    public void RemoveTransformComp(int ent)
    {
        SyLog.Debug(ELogTag.Ecs, $"remove transform comp from g{ent}");

        World.GetPool<TransformComp>().Del(ent);
        SyProxyEcs.GeRemoveTransformComp(_gameEntToEngineEnt[ent]);
        DecreaseEngineCompsCountAndTryDestroy(ent);
    }

    public ref MeshComp AddMeshComp(int ent)
    {
        SyLog.Debug(ELogTag.Ecs, $"add mesh comp to g{ent}");
        
        TryCreateEngineEntity(ent);
        
        ref var comp = ref World.GetPool<MeshComp>().Add(ent);
        
        SyProxyEcs.GeAddMeshComp(_gameEntToEngineEnt[ent]);
        IncreaseEngineCompsCount(ent);

        return ref comp;
    }

    public void RemoveMeshComp(int ent)
    {
        SyLog.Debug(ELogTag.Ecs, $"remove mesh comp from g{ent}");

        World.GetPool<MeshComp>().Add(ent);
        SyProxyEcs.GeRemoveMeshComp(_gameEntToEngineEnt[ent]);
        DecreaseEngineCompsCountAndTryDestroy(ent);
    }

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    internal uint ToEngineEnt(int gameEnt)
        => _gameEntToEngineEnt[gameEnt];
    internal bool ToEngineEnt(int gameEnt, out uint engineEnt)
        => _gameEntToEngineEnt.TryGetValue(gameEnt, out engineEnt);

    internal int ToGameEnt(uint engineEnt)
        => _engineEntToGameEnt[engineEnt];
    internal bool ToGameEnt(uint engineEnt, out int gameEnt)
        => _engineEntToGameEnt.TryGetValue(engineEnt, out gameEnt);
    
    //-----------------------------------------------------------
    //-----------------------------------------------------------
    public EcsWorld.Mask BuildFilter<T>() where T : struct
    {
        return World.Filter<T>();
    }

    public ref T AddSingleton<T>() where T : struct, IComp => ref AddSingletonRaw<T>();

    public ref T GetSingleton<T>() where T : struct
    {
        return ref World.GetPool<T>().Get(_singletonEntity);
    }
    
    public void RemoveSingleton<T>() where T: struct, IComp => RemoveSingletonRaw<T>();

    internal ref T AddSingletonRaw<T>() where T: struct
    {
        return ref World.GetPool<T>().Add(_singletonEntity);
    }

    internal void RemoveSingletonRaw<T>() where T : struct
    {
        World.GetPool<T>().Del(_singletonEntity);
    }


    //-----------------------------------------------------------
    //-----------------------------------------------------------
    private void TryCreateEngineEntity(int gameEnt)
    {
        if (_gameEntToEngineEnt.TryGetValue(gameEnt, out _))
            return;

        SyLog.Debug(ELogTag.Ecs, "create engine entity");
        uint engineEnt = SyProxyEcs.GeCreateEngineEntity();

        _gameEntToEngineEnt[gameEnt]        = engineEnt;
        _engineEntToGameEnt[engineEnt]      = gameEnt;
        _gameEntToEngineCompsCount[gameEnt] = 0;
        SyLog.Debug(ELogTag.Ecs, $"entities paired g{gameEnt} : e{engineEnt}");
    }

    private void TryDestroyEngineEntity(int gameEnt)
    {
        if (!_gameEntToEngineEnt.TryGetValue(gameEnt, out uint engineEnt))
            return;

        SyLog.Debug(ELogTag.Ecs, $"destroy entity e{engineEnt}");
        SyProxyEcs.GeDestroyEngineEntity(engineEnt);

        _gameEntToEngineEnt.Remove(gameEnt);
        _engineEntToGameEnt.Remove(engineEnt);
        _gameEntToEngineCompsCount.Remove(gameEnt);
    }

    private void IncreaseEngineCompsCount(int gameEnt)
    {
        _gameEntToEngineCompsCount[gameEnt] += 1;
    }

    private void DecreaseEngineCompsCountAndTryDestroy(int gameEnt)
    {
        _gameEntToEngineCompsCount[gameEnt] -= 1;
        if (_gameEntToEngineCompsCount[gameEnt] == 0)
            TryDestroyEngineEntity(gameEnt);
    }

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    public interface IComp { }
}
}
