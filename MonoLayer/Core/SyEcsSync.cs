using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.InteropServices;
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

    private readonly EcsPool<SceneObjectComp> _sceneObjectsPool;
    private readonly EcsPool<TransformComp>   _transformsPool;
    private readonly EcsPool<MeshComp>        _meshesPool;
    private readonly EcsPool<LightComp>       _lightsPool;
    private readonly EcsPool<ColliderComp>    _collidersPool;
    private readonly EcsPool<RigidComp>       _rigidsPool;
    private readonly EcsPool<SkyboxComp>      _skyboxesPool;
    private readonly EcsPool<ParticlesComp>   _particlesPool;
    private readonly EcsPool<SoundComp>       _soundsPool;

    private readonly EcsFilter _sceneObjectsFilter;
    private readonly EcsFilter _transformsFilter;
    private readonly EcsFilter _meshesFilter;
    private readonly EcsFilter _lightsFilter;
    private readonly EcsFilter _colliderFilter;
    private readonly EcsFilter _rigidsFilter;
    private readonly EcsFilter _skyboxesFilter;
    private readonly EcsFilter _particlesFilter;
    private readonly EcsFilter _soundsFilter;

    public SyEcsSync(SyEcs ecs)
    {
        _ecs = ecs;

        _sceneObjectsPool = ecs.World.GetPool<SceneObjectComp>();
        _transformsPool   = ecs.World.GetPool<TransformComp>();
        _meshesPool       = ecs.World.GetPool<MeshComp>();
        _lightsPool       = ecs.World.GetPool<LightComp>();
        _collidersPool    = ecs.World.GetPool<ColliderComp>();
        _rigidsPool       = ecs.World.GetPool<RigidComp>();
        _skyboxesPool     = ecs.World.GetPool<SkyboxComp>();
        _particlesPool    = ecs.World.GetPool<ParticlesComp>();
        _soundsPool       = ecs.World.GetPool<SoundComp>();

        _sceneObjectsFilter = ecs.World.Filter<SceneObjectComp>().End();
        _transformsFilter   = ecs.World.Filter<TransformComp>().End();
        _meshesFilter       = ecs.World.Filter<MeshComp>().End();
        _lightsFilter       = ecs.World.Filter<LightComp>().End();
        _colliderFilter     = ecs.World.Filter<ColliderComp>().End();
        _rigidsFilter       = ecs.World.Filter<RigidComp>().End();
        _skyboxesFilter     = ecs.World.Filter<SkyboxComp>().End();
        _particlesFilter    = ecs.World.Filter<ParticlesComp>().End();
        _soundsFilter       = ecs.World.Filter<SoundComp>().End();
    }

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    public void SyncEngineWithGame()
    {
        SendSceneObjectsToEngine();
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
        SendSkyboxesToEngine();
        SendParticlesToEngine();
        SendSoundsToEngine();
    }

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    private void SendSceneObjectsToEngine()
    {
        foreach (int ent in _sceneObjectsFilter)
        {
            ref var sceneObject = ref _sceneObjectsPool.Get(ent);

            int hash = sceneObject.GetHashCode();
            if (hash == sceneObject.Hash)
                continue;
            sceneObject.Hash = hash;

            var proxy = new ProxySceneObjectComp
            {
                Name     = sceneObject.Name,
                IsActive = sceneObject.IsActive
            };
            SyProxyEcs.GeUpdateSceneObjectComp(_ecs.ToEngineEnt(ent), proxy);
            
            Console.WriteLine($"[TEST] g{ent} scene object sent to engine");
        }
    }

    public void ReceiveSceneObjectFromEngine(uint engineEnt, ProxySceneObjectComp proxy)
    {
        int gameEnt = _ecs.GetOrCreateEntitiesPairByEngineEnt(engineEnt);
        // sceneObject is added automatically on entities pair create

        ref var comp = ref _sceneObjectsPool.Get(gameEnt);
        comp.Name     = proxy.Name;
        comp.IsActive = proxy.IsActive;

        comp.Hash = comp.GetHashCode();
        
        Console.WriteLine($"[TEST] g{gameEnt} scene object received from engine");
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
        // transform is added automatically on entities pair create
        
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
        if (!_meshesPool.Has(gameEnt))
            _meshesPool.Add(gameEnt);
        
        ref var mesh = ref _meshesPool.Get(gameEnt);

        mesh.Model = proxy.ModelUuid == null ? null : new ResRef<ResModel>(proxy.ModelUuid);

        if (mesh.Materials == null)
            mesh.Materials = new List<ResRef<ResMaterial>>();
        mesh.Materials.Clear();
        
        if (proxy.MaterialsUuids != null)
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
        if (!_lightsPool.Has(gameEnt))
            _lightsPool.Add(gameEnt);

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
        if (!_collidersPool.Has(gameEnt))
            _collidersPool.Add(gameEnt);

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
        if (!_rigidsPool.Has(gameEnt))
            _rigidsPool.Add(gameEnt);

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

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    private void SendSkyboxesToEngine()
    {
        foreach (int ent in _skyboxesFilter)
        {
            ref var skybox = ref _skyboxesPool.Get(ent);
            int     hash   = skybox.GetHashCode();
            if (hash == skybox.Hash)
                continue;
            skybox.Hash = hash;

            var proxy = new ProxySkyboxComp
            {
                CubemapUuid = skybox.Cubemap?.Uuid
            };
            
            SyProxyEcs.GeUpdateSkyboxComp(_ecs.ToEngineEnt(ent), proxy);
            
            Console.WriteLine($"[TEST] g{ent} skybox sent to engine");
        }
    }

    public void ReceiveSkyboxFromEngine(uint engineEnt, ProxySkyboxComp proxy)
    {
        int gameEnt = _ecs.GetOrCreateEntitiesPairByEngineEnt(engineEnt);
        if (!_skyboxesPool.Has(gameEnt))
            _skyboxesPool.Add(gameEnt);

        ref var skybox = ref _skyboxesPool.Get(gameEnt);
        skybox.Cubemap = proxy.CubemapUuid == null ? null : new ResRef<ResCubemap>(proxy.CubemapUuid);

        skybox.Hash = skybox.GetHashCode();
        
        Console.WriteLine($"[TEST] g{gameEnt} skybox received from engine");
    }

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    private void SendParticlesToEngine()
    {
        var proxy = new ProxyParticlesComp();
        
        foreach (int ent in _particlesFilter)
        {
            ref var particles = ref _particlesPool.Get(ent);
            if (!particles.IsDirty)
                continue;
            particles.IsDirty = false;

            proxy.Duration             = particles.Duration;
            proxy.IsLooping            = particles.IsLooping;
            proxy.StartDelayTime       = particles.StartDelayTime;
            proxy.StartLifeTime        = particles.StartLifeTime;
            proxy.StartSpeed           = particles.StartSpeed;
            proxy.StartSize            = particles.StartSize;
            proxy.StartColor           = particles.StartColor;
            proxy.StartRotation        = particles.StartRotation;
            proxy.SizeOverLifetime     = particles.SizeOverLifetime;
            proxy.SpeedOverLifetime    = particles.SpeedOverLifetime;
            proxy.RotationOverLifetime = particles.RotationOverLifetime;
            proxy.MaxParticles         = particles.MaxParticles < 0 ? 0 : (uint)particles.MaxParticles;
            proxy.IsLit                = particles.IsLit;
            proxy.AmbientAmount        = particles.AmbientAmount;

            proxy.RateOverTime = particles.RateOverTime;

            if (particles.Bursts == null)
            {
                proxy.BurstsCount = 0;
                proxy.Bursts      = IntPtr.Zero;
            }
            else
            {
                proxy.BurstsCount = particles.Bursts.Count;
                int burstSize = Marshal.SizeOf<ParticlesComp.BurstData>();
                proxy.Bursts = Marshal.AllocHGlobal(burstSize * proxy.BurstsCount);
                var ptr = proxy.Bursts;
                for (var i = 0; i < proxy.BurstsCount; i++)
                {
                    Marshal.StructureToPtr(particles.Bursts[i], ptr, true);
                    ptr += burstSize;
                }
            }

            proxy.EmitShape = particles.EmitShape;
            proxy.Angle     = particles.Angle;
            proxy.Radius    = particles.Radius;

            proxy.TextureUuid = particles.Texture?.Uuid;
            
            SyProxyEcs.GeUpdateParticlesComp(_ecs.ToEngineEnt(ent), proxy);
            
            if (proxy.Bursts != IntPtr.Zero)
                Marshal.FreeHGlobal(proxy.Bursts);
            
            Console.WriteLine($"[TEST] g{ent} particles sent to engine");
        }
    }

    public void ReceiveParticlesFromEngine(uint engineEnt, ProxyParticlesComp proxy)
    {
        int gameEnt = _ecs.GetOrCreateEntitiesPairByEngineEnt(engineEnt);
        if (!_particlesPool.Has(gameEnt))
            _particlesPool.Add(gameEnt);

        ref var particles = ref _particlesPool.Get(gameEnt);
        
        particles.Duration             = proxy.Duration;
        particles.IsLooping            = proxy.IsLooping;
        particles.StartDelayTime       = proxy.StartDelayTime;
        particles.StartLifeTime        = proxy.StartLifeTime;
        particles.StartSpeed           = proxy.StartSpeed;
        particles.StartSize            = proxy.StartSize;
        particles.StartColor           = proxy.StartColor;
        particles.StartRotation        = proxy.StartRotation;
        particles.SizeOverLifetime     = proxy.SizeOverLifetime;
        particles.SpeedOverLifetime    = proxy.SpeedOverLifetime;
        particles.RotationOverLifetime = proxy.RotationOverLifetime;
        particles.MaxParticles         = (int)proxy.MaxParticles;
        particles.IsLit                = proxy.IsLit;
        particles.AmbientAmount        = proxy.AmbientAmount;

        particles.RateOverTime = proxy.RateOverTime;
        
        if (particles.Bursts == null)
            particles.Bursts = new List<ParticlesComp.BurstData>();
        else
            particles.Bursts.Clear();
        int burstSize = Marshal.SizeOf<ParticlesComp.BurstData>();
        for (var i = 0; i < proxy.BurstsCount; i++)
        {
            var ptr = proxy.Bursts + i * burstSize;
            particles.Bursts.Add(Marshal.PtrToStructure<ParticlesComp.BurstData>(ptr));
        }

        particles.EmitShape = proxy.EmitShape;
        particles.Angle     = proxy.Angle;
        particles.Radius    = proxy.Radius;

        if (particles.Texture?.Uuid != proxy.TextureUuid)
            particles.Texture = proxy.TextureUuid == null ? null : new ResRef<ResTexture>(proxy.TextureUuid);

        particles.IsDirty = false;
        
        Console.WriteLine($"[TEST] g{gameEnt} particles received from engine");
    }

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    private void SendSoundsToEngine()
    {
        foreach (int ent in _soundsFilter)
        {
            ref var sound = ref _soundsPool.Get(ent);
            int     hash   = sound.GetHashCode();
            if (hash == sound.Hash)
                continue;
            sound.Hash = hash;

            var proxy = new ProxySoundComp
            {
                IsPlaying = sound.IsPlaying,
                IsLooping = sound.IsLooping,
                Is3d      = sound.Is3d,
                Volume    = sound.Volume,
                SoundUuid = sound.Sound?.Uuid
            };
            
            SyProxyEcs.GeUpdateSoundComp(_ecs.ToEngineEnt(ent), proxy);
            
            Console.WriteLine($"[TEST] g{ent} sound sent to engine");
        }
    }

    public void ReceiveSoundFromEngine(uint engineEnt, ProxySoundComp proxy)
    {
        int gameEnt = _ecs.GetOrCreateEntitiesPairByEngineEnt(engineEnt);
        if (!_soundsPool.Has(gameEnt))
            _soundsPool.Add(gameEnt);

        ref var sound = ref _soundsPool.Get(gameEnt);
        sound.IsPlaying = proxy.IsPlaying;
        sound.IsLooping = proxy.IsLooping;
        sound.Is3d      = proxy.Is3d;
        sound.Volume    = proxy.Volume;
        sound.Sound     = proxy.SoundUuid == null ? null : new ResRef<ResSound>(proxy.SoundUuid);

        sound.Hash = sound.GetHashCode();
        
        Console.WriteLine($"[TEST] g{gameEnt} sound received from engine");
    }
}
}
