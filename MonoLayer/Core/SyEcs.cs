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


    public ref T AddComp<T>(int ent) where T : struct, IGameComp
    {
        return ref World.GetPool<T>().Add(ent);
    }

    public void RemoveComp<T>(int ent) where T : struct, IGameComp
    {
        World.GetPool<T>().Del(ent);
    }


    public ref T GetComp<T>(int ent) where T : struct, IComp
    {
        return ref World.GetPool<T>().Get(ent);
    }


    public ref TransformComp AddTransformComp(int ent)
    {
        SyLog.Debug(ELogTag.Ecs, $"add transform comp to g{ent}");

        TryCreateEngineEntity(ent);

        ref var comp = ref World.GetPool<TransformComp>().Add(ent);
        comp.Scale      = SyVector3.One;
        comp.LocalScale = SyVector3.One;

        SyProxyEcs.GeAddEngineComp(_gameEntToEngineEnt[ent], SyProxyEcs.EEngineCompId.Transform);
        IncreaseEngineCompsCount(ent);

        return ref comp;
    }

    public void RemoveTransformComp(int ent)
    {
        SyLog.Debug(ELogTag.Ecs, $"remove transform comp from g{ent}");

        World.GetPool<TransformComp>().Del(ent);
        SyProxyEcs.GeRemoveEngineComp(_gameEntToEngineEnt[ent], SyProxyEcs.EEngineCompId.Transform);
        DecreaseEngineCompsCountAndTryDestroy(ent);
    }

    public ref MeshComp AddMeshComp(int ent)
    {
        SyLog.Debug(ELogTag.Ecs, $"add mesh comp to g{ent}");

        TryCreateEngineEntity(ent);

        ref var comp = ref World.GetPool<MeshComp>().Add(ent);

        SyProxyEcs.GeAddEngineComp(_gameEntToEngineEnt[ent], SyProxyEcs.EEngineCompId.Mesh);
        IncreaseEngineCompsCount(ent);

        return ref comp;
    }

    public void RemoveMeshComp(int ent)
    {
        SyLog.Debug(ELogTag.Ecs, $"remove mesh comp from g{ent}");

        World.GetPool<MeshComp>().Del(ent);
        SyProxyEcs.GeRemoveEngineComp(_gameEntToEngineEnt[ent], SyProxyEcs.EEngineCompId.Mesh);
        DecreaseEngineCompsCountAndTryDestroy(ent);
    }

    public ref LightComp AddLightComp(int ent)
    {
        SyLog.Debug(ELogTag.Ecs, $"add light comp to g{ent}");

        TryCreateEngineEntity(ent);

        ref var comp = ref World.GetPool<LightComp>().Add(ent);
        comp.Type              = LightComp.EType.PointLight;
        comp.Behaviour         = LightComp.EBehaviour.Movable;
        comp.Color             = SyColor.White;
        comp.PointLightRadius  = 1;
        comp.ShouldCastShadows = false;

        SyProxyEcs.GeAddEngineComp(_gameEntToEngineEnt[ent], SyProxyEcs.EEngineCompId.Light);
        IncreaseEngineCompsCount(ent);

        return ref comp;
    }

    public void RemoveLightComp(int ent)
    {
        SyLog.Debug(ELogTag.Ecs, $"remove light comp from g{ent}");

        World.GetPool<LightComp>().Del(ent);
        SyProxyEcs.GeRemoveEngineComp(_gameEntToEngineEnt[ent], SyProxyEcs.EEngineCompId.Light);
        DecreaseEngineCompsCountAndTryDestroy(ent);
    }

    public ref ColliderComp AddColliderComp(int ent)
    {
        SyLog.Debug(ELogTag.Ecs, $"add collider comp to g{ent}");

        TryCreateEngineEntity(ent);

        ref var comp = ref World.GetPool<ColliderComp>().Add(ent);
        comp.Type = ColliderComp.EType.Box;
        comp.Extent = SyVector3.One;

        SyProxyEcs.GeAddEngineComp(_gameEntToEngineEnt[ent], SyProxyEcs.EEngineCompId.Collider);
        IncreaseEngineCompsCount(ent);

        return ref comp;
    }

    public void RemoveCollider(int ent)
    {
        SyLog.Debug(ELogTag.Ecs, $"remove collider comp from g{ent}");

        World.GetPool<ColliderComp>().Del(ent);
        SyProxyEcs.GeRemoveEngineComp(_gameEntToEngineEnt[ent], SyProxyEcs.EEngineCompId.Collider);
        DecreaseEngineCompsCountAndTryDestroy(ent);
    }
    

    public ref RigidComp AddRigidComp(int ent)
    {
        SyLog.Debug(ELogTag.Ecs, $"add rigid comp to g{ent}");

        TryCreateEngineEntity(ent);

        ref var comp = ref World.GetPool<RigidComp>().Add(ent);
        comp.Type = RigidComp.EType.Dynamic;
        comp.Mass = 1f;
        //comp.IsAutoMass  = true;
        comp.IsGravityOn = true;

        SyProxyEcs.GeAddEngineComp(_gameEntToEngineEnt[ent], SyProxyEcs.EEngineCompId.Rigid);
        IncreaseEngineCompsCount(ent);

        return ref comp;
    }

    public void RemoveRigidComp(int ent)
    {
        SyLog.Debug(ELogTag.Ecs, $"remove rigid comp from g{ent}");

        World.GetPool<RigidComp>().Del(ent);
        SyProxyEcs.GeRemoveEngineComp(_gameEntToEngineEnt[ent], SyProxyEcs.EEngineCompId.Rigid);
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

    public ref T AddSingleton<T>() where T : struct, IGameComp => ref AddSingletonRaw<T>();

    public ref T GetSingleton<T>() where T : struct
    {
        return ref World.GetPool<T>().Get(_singletonEntity);
    }

    public void RemoveSingleton<T>() where T : struct, IGameComp => RemoveSingletonRaw<T>();

    internal ref T AddSingletonRaw<T>() where T : struct, IComp
    {
        return ref World.GetPool<T>().Add(_singletonEntity);
    }

    internal void RemoveSingletonRaw<T>() where T : struct, IComp
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
    public interface IGameComp : IComp { }
}
}
