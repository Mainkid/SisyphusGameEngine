using System;
using System.Collections.Generic;
using Leopotam.EcsLite;
using SyEngine.Core.Comps;
using SyEngine.Core.Datas;
using SyEngine.Core.ProxyComps;
using SyEngine.Core.Resources;

namespace SyEngine.Core
{
internal class SyEcsSync
{
    private readonly SyEcs _ecs;

    private readonly EcsPool<TransformComp> _transformsPool;
    private readonly EcsPool<MeshComp>      _meshesPool;
    private readonly EcsPool<LightComp>     _lightsPool;
    private readonly EcsPool<ColliderComp>  _collidersPool;
    private readonly EcsPool<RigidComp>     _rigidsPool;

    private readonly EcsFilter _transformsFilter;
    private readonly EcsFilter _meshesFilter;
    private readonly EcsFilter _lightsFilter;
    private readonly EcsFilter _colliderFilter;
    private readonly EcsFilter _rigidsFilter;

    public SyEcsSync(SyEcs ecs)
    {
        _ecs = ecs;

        _transformsPool = ecs.World.GetPool<TransformComp>();
        _meshesPool     = ecs.World.GetPool<MeshComp>();
        _lightsPool     = ecs.World.GetPool<LightComp>();
        _collidersPool  = ecs.World.GetPool<ColliderComp>();
        _rigidsPool     = ecs.World.GetPool<RigidComp>();

        _transformsFilter = ecs.World.Filter<TransformComp>().End();
        _meshesFilter     = ecs.World.Filter<MeshComp>().End();
        _lightsFilter     = ecs.World.Filter<LightComp>().End();
        _colliderFilter   = ecs.World.Filter<ColliderComp>().End();
        _rigidsFilter     = ecs.World.Filter<RigidComp>().End();
    }

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    public void SyncEngineWithGame()
    {
        //Console.WriteLine("[TEST] send transforms");
        SendTransformsToEngine();
        //Console.WriteLine("[TEST] send meshes");
        SendMeshesToEngine();
        //Console.WriteLine("[TEST] send lights");
        SendLightsToEngine();
        //Console.WriteLine("[TEST] send colliders");
        SendCollidersToEngine();
        //Console.WriteLine("[TEST] send rigids");
        SendRigidsToEngine();
    }

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    private void SendTransformsToEngine()
    {
        foreach (int ent in _transformsFilter)
        {
            ref var tf = ref _transformsPool.Get(ent);

            int hash = tf.GetHashCode();
            if (hash == tf.Hash)
                continue;
            tf.Hash = hash;

            uint engineParentEnt = default;
            bool hasParent = tf.ParentEnt != null &&
                             _ecs.ToEngineEnt(tf.ParentEnt.Value, out engineParentEnt);

            var proxy = new ProxyTransformComp
            {
                Position        = tf.Position,
                Rotation        = tf.Rotation,
                Scale           = tf.Scale,
                LocalPosition   = tf.LocalPosition,
                LocalRotation   = tf.LocalRotation,
                LocalScale      = tf.LocalScale,
                HasParent       = hasParent,
                ParentEngineEnt = engineParentEnt
            };
            SyProxyEcs.GeUpdateTransformComp(_ecs.ToEngineEnt(ent), proxy);
            
            Console.WriteLine($"[TEST] g{ent} transform sent to engine");
        }
    }

    public void ReceiveTransformFromEngine(uint engineEnt, ProxyTransformComp proxy)
    {
        int gameEnt = _ecs.GetOrCreateEntitiesPairByEngineEnt(engineEnt);
        _ecs.CreateCompByEngineIfNone<TransformComp>(gameEnt);
        
        ref var tf = ref _transformsPool.Get(gameEnt);
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

        tf.Hash = tf.GetHashCode();
        
        Console.WriteLine($"[TEST] g{gameEnt} transform received from engine");
    }

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    private void SendMeshesToEngine()
    {
        foreach (int ent in _meshesFilter)
        {
            ref var mesh = ref _meshesPool.Get(ent);
            
            int hash = mesh.GetHashCode();
            if (hash == mesh.Hash)
                continue;
            mesh.Hash = hash;

            var proxy = new ProxyMeshComp();
            proxy.ModelUuid = mesh.Model?.Uuid;
            if (mesh.Materials == null || mesh.Materials.Count == 0)
            {
                proxy.MaterialsUuids = null;
            }
            else
            {
                proxy.MaterialsUuids = new string[mesh.Materials.Count];
                for (var i = 0; i < mesh.Materials.Count; i++)
                    proxy.MaterialsUuids[i] = mesh.Materials[i]?.Uuid;
            }
            
            SyProxyEcs.GeUpdateMeshComp(_ecs.ToEngineEnt(ent), proxy);
            
            Console.WriteLine($"[TEST] g{ent} mesh sent to engine");
        }
    }

    public void ReceiveMeshFromEngine(uint engineEnt, ProxyMeshComp proxy)
    {
        int gameEnt = _ecs.GetOrCreateEntitiesPairByEngineEnt(engineEnt);
        _ecs.CreateCompByEngineIfNone<MeshComp>(gameEnt);
        
        ref var mesh = ref _meshesPool.Get(gameEnt);

        mesh.Model = proxy.ModelUuid == null ? null : new ResRef<ResModel>(proxy.ModelUuid);

        if (mesh.Materials == null)
            mesh.Materials = new List<ResRef<ResMaterial>>();
        mesh.Materials.Clear();
        foreach (string uuid in proxy.MaterialsUuids)
            mesh.Materials.Add(new ResRef<ResMaterial>(uuid));

        mesh.Hash = mesh.GetHashCode();
        
        Console.WriteLine($"[TEST] g{gameEnt} mesh received from engine");
    }

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    private void SendLightsToEngine()
    {
        foreach (int ent in _lightsFilter)
        {
            ref var light = ref _lightsPool.Get(ent);

            int hash = light.GetHashCode();
            if (hash == light.Hash)
                continue;
            light.Hash = hash;

            var proxy = new ProxyLightComp
            {
                Type             = light.Type,
                Behaviour        = light.Behaviour,
                Color            = light.Color,
                PointLightRadius = light.PointLightRadius,
                ShouldCastShadows = light.ShouldCastShadows
            };
            
            SyProxyEcs.GeUpdateLightComp(_ecs.ToEngineEnt(ent), proxy);
            
            Console.WriteLine($"[TEST] g{ent} light sent to engine");
        }
    }

    public void ReceiveLightFromEngine(uint engineEnt, ProxyLightComp proxy)
    {
        int gameEnt = _ecs.GetOrCreateEntitiesPairByEngineEnt(engineEnt);
        _ecs.CreateCompByEngineIfNone<LightComp>(gameEnt);

        ref var light = ref _lightsPool.Get(gameEnt);
        light.Type              = proxy.Type;
        light.Behaviour         = proxy.Behaviour;
        light.Color             = new SyColor(proxy.Color.X, proxy.Color.Y, proxy.Color.Z, proxy.Color.W);
        light.PointLightRadius  = proxy.PointLightRadius;
        light.ShouldCastShadows = proxy.ShouldCastShadows;
        
        light.Hash = light.GetHashCode();
        
        Console.WriteLine($"[TEST] g{gameEnt} light received from engine");
    }

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    private void SendCollidersToEngine()
    {
        foreach (int ent in _colliderFilter)
        {
            ref var collider = ref _collidersPool.Get(ent);

            int hash = collider.GetHashCode();
            if (hash == collider.Hash)
                continue;
            collider.Hash = hash;

            var proxy = new ProxyColliderComp
            {
                Type       = collider.Type,
                Extent     = collider.Extent,
                Radius     = collider.Radius,
                HalfHeight = collider.HalfHeight
            };
            SyProxyEcs.GeUpdateColliderComp(_ecs.ToEngineEnt(ent), proxy);
            
            Console.WriteLine($"[TEST] g{ent} collider sent to engine");
        }
    }

    public void ReceiveColliderFromEngine(uint engineEnt, ProxyColliderComp proxy)
    {
        int gameEnt = _ecs.GetOrCreateEntitiesPairByEngineEnt(engineEnt);
        _ecs.CreateCompByEngineIfNone<ColliderComp>(gameEnt);

        ref var collider = ref _collidersPool.Get(gameEnt);
        collider.Type       = proxy.Type;
        collider.Extent     = proxy.Extent;
        collider.Radius     = proxy.Radius;
        collider.HalfHeight = proxy.HalfHeight;

        collider.Hash = collider.GetHashCode();
        
        Console.WriteLine($"[TEST] g{gameEnt} collider received from engine");
    }

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    private void SendRigidsToEngine()
    {
        foreach (int ent in _rigidsFilter)
        {
            ref var rigid = ref _rigidsPool.Get(ent);

            int hash = rigid.GetHashCode();
            if (hash == rigid.Hash)
                continue;
            rigid.Hash = hash;

            var proxy = new ProxyRigidComp
            {
                Type            = rigid.Type,
                Mass            = rigid.Mass,
                IsAutoMass      = rigid.IsAutoMass,
                IsKinematic     = rigid.IsKinematic,
                IsGravityOn     = rigid.IsGravityOn,
                LinearVelocity  = rigid.LinearVelocity,
                AngularVelocity = rigid.AngularVelocity
            };
            
            SyProxyEcs.GeUpdateRigidComp(_ecs.ToEngineEnt(ent), proxy);
            
            Console.WriteLine($"[TEST] g{ent} rigid sent to engine");
        }
    }

    public void ReceiveRigidFromEngine(uint engineEnt, ProxyRigidComp proxy)
    {
        int gameEnt = _ecs.GetOrCreateEntitiesPairByEngineEnt(engineEnt);
        _ecs.CreateCompByEngineIfNone<RigidComp>(gameEnt);

        ref var rigid = ref _rigidsPool.Get(gameEnt);

        rigid.Type            = proxy.Type;
        rigid.Mass            = proxy.Mass;
        rigid.IsAutoMass      = proxy.IsAutoMass;
        rigid.IsKinematic     = proxy.IsKinematic;
        rigid.IsGravityOn     = proxy.IsGravityOn;
        rigid.LinearVelocity  = proxy.LinearVelocity;
        rigid.AngularVelocity = proxy.AngularVelocity;

        rigid.Hash = rigid.GetHashCode();
        
        Console.WriteLine($"[TEST] g{gameEnt} rigid received from engine");
    }
}
}
