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

    public void DestroyEntity(int gameEnt)
    {
        World.DelEntity(gameEnt);
        DestroyEntitiesPairIfExists(gameEnt);
    }
    
    
    internal void DestroyGameEntityByEngine(uint engineEnt)
    {
        if (!_engineEntToGameEnt.TryGetValue(engineEnt, out int gameEnt))
            return;

        SyLog.Debug(ELogTag.Ecs, $"g{gameEnt} destroy by engine");
        World.DelEntity(gameEnt);

        _gameEntToEngineEnt.Remove(gameEnt);
        _engineEntToGameEnt.Remove(engineEnt);
        _gameEntToEngineCompsCount.Remove(gameEnt);
    }


    //-----------------------------------------------------------
    //-----------------------------------------------------------
    public ref T AddComp<T>(int ent) where T : struct, IComp
    {
        var engineCompId = EngineCompIdHelper.GetFromCompType(typeof(T));
        if (engineCompId != null)
        {
            uint engineEnt = GetOrCreateEntitiesPairByGameEnt(ent);
            SyLog.Debug(ELogTag.Ecs, $"add {engineCompId} to g{ent}");
            SyProxyEcs.GeAddEngineComp(engineEnt, engineCompId.Value);
            IncreaseEngineCompsCount(ent);
        }

        ref var comp = ref World.GetPool<T>().Add(ent);
        
        switch (engineCompId)
        {
            case EEngineCompId.Transform:
                ref var tf = ref World.GetPool<TransformComp>().Get(ent);
                tf.Scale      = SyVector3.One;
                tf.LocalScale = SyVector3.One;
                break;
            case EEngineCompId.Mesh:
                break;
            case EEngineCompId.Light:
                ref var light = ref World.GetPool<LightComp>().Get(ent);
                light.Type              = LightComp.EType.PointLight;
                light.Behaviour         = LightComp.EBehaviour.Movable;
                light.Color             = SyColor.White;
                light.PointLightRadius  = 1;
                light.ShouldCastShadows = false;
                break;
            case EEngineCompId.Collider:
                ref var collider = ref World.GetPool<ColliderComp>().Get(ent);
                collider.Type   = ColliderComp.EType.Box;
                collider.Extent = SyVector3.One;
                break;
            case EEngineCompId.Rigid:
                ref var rigid = ref World.GetPool<RigidComp>().Get(ent);
                rigid.Type = RigidComp.EType.Dynamic;
                rigid.Mass = 1f;
                //comp.IsAutoMass  = true;
                rigid.IsGravityOn = true;
                break;
            case null:
                break;
            default:
                throw new Exception($"{engineCompId} is not implemented");
        }

        return ref comp;
    }

    public void RemoveComp<T>(int ent) where T : struct, IComp
    {
        var engineCompId = EngineCompIdHelper.GetFromCompType(typeof(T));
        
        World.GetPool<T>().Del(ent);

        if (engineCompId != null)
        {
            SyLog.Debug(ELogTag.Ecs, $"remove {engineCompId} comp from g{ent}");
            SyProxyEcs.GeRemoveEngineComp(_gameEntToEngineEnt[ent], engineCompId.Value);
            DecreaseEngineCompsCount(ent);
        }
    }

    public ref T GetComp<T>(int ent) where T : struct, IComp
    {
        return ref World.GetPool<T>().Get(ent);
    }
    
    internal void AddCompRaw(Type compType, int ent)
    {
        if (compType == typeof(TransformComp))
            AddComp<TransformComp>(ent);
        else if (compType == typeof(MeshComp))
            AddComp<MeshComp>(ent);
        else if (compType == typeof(LightComp))
            AddComp<LightComp>(ent);
        else if (compType == typeof(ColliderComp))
            AddComp<ColliderComp>(ent);
        else if (compType == typeof(RigidComp))
            AddComp<RigidComp>(ent);
        else
            World.GetPoolByType(compType).AddRaw(ent, Activator.CreateInstance(compType));
    }

    internal void RemoveCompRaw(Type compType, int ent)
    {
        if (compType == typeof(TransformComp))
            RemoveComp<TransformComp>(ent);
        else if (compType == typeof(MeshComp))
            RemoveComp<MeshComp>(ent);
        else if (compType == typeof(LightComp))
            RemoveComp<LightComp>(ent);
        else if (compType == typeof(ColliderComp))
            RemoveComp<ColliderComp>(ent);
        else if (compType == typeof(RigidComp))
            RemoveComp<RigidComp>(ent);
        else
            World.GetPoolByType(compType).Del(ent);
    }

    internal void CreateCompByEngineIfNone<T>(int gameEnt) where T: struct, IComp
    {
        var pool = World.GetPool<T>();
        if (!pool.Has(gameEnt))
        {
            pool.Add(gameEnt);
            IncreaseEngineCompsCount(gameEnt);
        }
    }

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    public EcsWorld.Mask BuildFilter<T>() where T : struct
    {
        return World.Filter<T>();
    }

    public ref T AddSingleton<T>() where T : struct, ISingletonGameComp => ref AddSingletonRaw<T>();

    public ref T GetSingleton<T>() where T : struct
    {
        return ref World.GetPool<T>().Get(_singletonEntity);
    }

    public void RemoveSingleton<T>() where T : struct, ISingletonGameComp => RemoveSingletonRaw<T>();

    internal ref T AddSingletonRaw<T>() where T : struct, ISingletonComp
    {
        return ref World.GetPool<T>().Add(_singletonEntity);
    }

    internal void RemoveSingletonRaw<T>() where T : struct, ISingletonComp
    {
        World.GetPool<T>().Del(_singletonEntity);
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

    private uint GetOrCreateEntitiesPairByGameEnt(int gameEnt)
    {
        if (_gameEntToEngineEnt.TryGetValue(gameEnt, out uint engineEnt))
            return engineEnt;

        SyLog.Debug(ELogTag.Ecs, "create engine entity");
        engineEnt = SyProxyEcs.GeCreateEngineEntity();

        _gameEntToEngineEnt[gameEnt]        = engineEnt;
        _engineEntToGameEnt[engineEnt]      = gameEnt;
        _gameEntToEngineCompsCount[gameEnt] = 0;
        SyLog.Debug(ELogTag.Ecs, $"entities paired g{gameEnt} : e{engineEnt}");

        return engineEnt;
    }
    
    internal int GetOrCreateEntitiesPairByEngineEnt(uint engineEnt)
    {
        if (_engineEntToGameEnt.TryGetValue(engineEnt, out int gameEnt))
            return gameEnt;

        gameEnt = World.NewEntity();

        _gameEntToEngineEnt[gameEnt]        = engineEnt;
        _engineEntToGameEnt[engineEnt]      = gameEnt;
        _gameEntToEngineCompsCount[gameEnt] = 0;
        SyLog.Debug(ELogTag.Ecs, $"entities paired g{gameEnt} : e{engineEnt}");
        
        return gameEnt;
    }
    

    private void DestroyEntitiesPairIfExists(int gameEnt)
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

    private void DecreaseEngineCompsCount(int gameEnt)
    {
        _gameEntToEngineCompsCount[gameEnt] -= 1;
        if (_gameEntToEngineCompsCount[gameEnt] == 0)
            DestroyEntitiesPairIfExists(gameEnt);
    }
    
    //-----------------------------------------------------------
    //-----------------------------------------------------------
    public interface IComp { }
    public interface ISingletonComp { }
    public interface ISingletonGameComp : ISingletonComp { }
}
}
