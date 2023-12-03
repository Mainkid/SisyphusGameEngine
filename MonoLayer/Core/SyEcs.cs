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

    private readonly Dictionary<int, uint> _gameEntToEngineEnt = new Dictionary<int, uint>();
    private readonly Dictionary<uint, int> _engineEntToGameEnt = new Dictionary<uint, int>();

    internal SyEcs()
    {
        World = new EcsWorld();

        _singletonEntity = World.NewEntity();
        World.GetPool<SingletonsTag>().Add(_singletonEntity);

        _sceneObjectsPool      = World.GetPool<SceneObjectComp>();
        _transformsPool = World.GetPool<TransformComp>();
    }

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    private List<SyEcsSystemBase> _systems = new List<SyEcsSystemBase>();

    private int _singletonEntity;

    private EcsPool<SceneObjectComp> _sceneObjectsPool;
    private EcsPool<TransformComp>   _transformsPool;


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
    public int CreateEntity(bool isSceneObject)
    {
        int ent = World.NewEntity();
        if (isSceneObject)
            CreateEntitiesPairByGameEnt(ent);
        return ent;
    }

    public void DestroyEntity(int gameEnt)
    {
        World.DelEntity(gameEnt);

        if (_gameEntToEngineEnt.TryGetValue(gameEnt, out uint engineEnt))
        {
            SyLog.Debug(ELogTag.Ecs, $"destroy e{engineEnt}");
            SyProxyEcs.GeDestroyEngineEntity(engineEnt);

            _gameEntToEngineEnt.Remove(gameEnt);
            _engineEntToGameEnt.Remove(engineEnt);
        }
    }
    
    
    internal void DestroyGameEntityByEngine(uint engineEnt)
    {
        if (!_engineEntToGameEnt.TryGetValue(engineEnt, out int gameEnt))
            return;

        SyLog.Debug(ELogTag.Ecs, $"destroy g{gameEnt} by engine");
        World.DelEntity(gameEnt);

        _gameEntToEngineEnt.Remove(gameEnt);
        _engineEntToGameEnt.Remove(engineEnt);
    }


    //-----------------------------------------------------------
    //-----------------------------------------------------------
    public ref T AddComp<T>(int ent) where T : struct, IComp
    {
        var engineCompId = EngineCompIdHelper.GetFromCompType(typeof(T));
        if (engineCompId != null)
        {
            if (!_gameEntToEngineEnt.TryGetValue(ent, out uint engineEnt))
                throw new Exception($"cannot add scene component {engineCompId} to non-scene entity {ent}");
            SyLog.Debug(ELogTag.Ecs, $"add {engineCompId} to g{ent}");
            SyProxyEcs.GeAddEngineComp(engineEnt, engineCompId.Value);
        }

        ref var comp = ref World.GetPool<T>().Add(ent);
        
        switch (engineCompId)
        {
            case EEngineCompId.SceneObject: //impossible
                break;
            case EEngineCompId.Transform: //impossible
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
        World.GetPool<T>().Del(ent);
        
        var engineCompId = EngineCompIdHelper.GetFromCompType(typeof(T));
        if (engineCompId != null)
        {
            SyLog.Debug(ELogTag.Ecs, $"remove {engineCompId} comp from g{ent}");
            SyProxyEcs.GeRemoveEngineComp(_gameEntToEngineEnt[ent], engineCompId.Value);
        }
    }

    public bool HasComp<T>(int ent) where T : struct, IComp
    {
        return World.GetPool<T>().Has(ent);
    }
    
    public ref T GetComp<T>(int ent) where T : struct, IInternalComp
    {
        return ref World.GetPool<T>().Get(ent);
    }

    public ref T GetOrAddComp<T>(int ent) where T : struct, IComp
    {
        if (HasComp<T>(ent))
            return ref GetComp<T>(ent);
        return ref AddComp<T>(ent);
    }
    
    
    internal void AddCompRaw(Type compType, int ent)
    {
        if (compType == typeof(SceneObjectComp) || 
            compType == typeof(TransformComp))
            throw new Exception($"cannot add {compType.Name}");
        
        if (compType == typeof(MeshComp))
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
        if (compType == typeof(SceneObjectComp) || 
            compType == typeof(TransformComp))
            throw new Exception($"cannot remove {compType.Name}");
        
        if (compType == typeof(MeshComp))
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

    private void CreateEntitiesPairByGameEnt(int gameEnt)
    {
        ref var sceneObject = ref _sceneObjectsPool.Add(gameEnt);
        sceneObject.Name     = "New Entity";
        sceneObject.IsActive = true;
        
        ref var tf = ref _transformsPool.Add(gameEnt);
        tf.Scale      = SyVector3.One;
        tf.LocalScale = SyVector3.One;
        
        SyLog.Debug(ELogTag.Ecs, "create engine entity");
        uint engineEnt = SyProxyEcs.GeCreateEngineEntity();

        _gameEntToEngineEnt[gameEnt]   = engineEnt;
        _engineEntToGameEnt[engineEnt] = gameEnt;
        SyLog.Debug(ELogTag.Ecs, $"entities paired g{gameEnt} : e{engineEnt}");
    }
    
    internal int GetOrCreateEntitiesPairByEngineEnt(uint engineEnt)
    {
        if (_engineEntToGameEnt.TryGetValue(engineEnt, out int gameEnt))
            return gameEnt;

        gameEnt = World.NewEntity();
        ref var sceneObject = ref _sceneObjectsPool.Add(gameEnt);
        sceneObject.Name     = "New Entity";
        sceneObject.IsActive = true;
        
        ref var tf = ref _transformsPool.Add(gameEnt);
        tf.Scale      = SyVector3.One;
        tf.LocalScale = SyVector3.One;

        _gameEntToEngineEnt[gameEnt]   = engineEnt;
        _engineEntToGameEnt[engineEnt] = gameEnt;
        SyLog.Debug(ELogTag.Ecs, $"entities paired g{gameEnt} : e{engineEnt}");
        
        return gameEnt;
    }
    
    //-----------------------------------------------------------
    //-----------------------------------------------------------
    public interface IInternalComp { }
    public interface IComp : IInternalComp
    {
    }
    
    public interface ISingletonComp { }
    public interface ISingletonGameComp : ISingletonComp { }
}
}
