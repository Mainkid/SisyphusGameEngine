using System;
using System.Runtime.CompilerServices;
using SyEngine.Game.Comps;
using SyEngine.Logger;

namespace SyEngine.Game
{
internal class SyProxyGame
{
    private readonly SyGameContext _context;

    private readonly SyEcs     _ecs;
    private readonly SyEcsSync _ecsSync;

    public SyProxyGame()
    {
        try
        {
            _context = new SyGameContext();

            _ecs     = new SyEcs(_context);
            _ecsSync = new SyEcsSync(_ecs, _context);

            _ecs.AddSingletonInternal<TimeData>();
        }
        catch (Exception e)
        {
            Console.WriteLine(e);
        }
    }

    public void EG_SetConfig(SyGameConfigBase config)
    {
        SyLog.Debug(ELogTag.ProxyGame, "config received");
        try
        {
            _ecs.SetSystems(config.GetSystems());
        }
        catch (Exception e)
        {
            SyLog.Err(ELogTag.ProxyGame, e.ToString());
        }
    }

    public void EG_Init()
    {
        try
        {
            _ecs.InitSystems();
            _ecsSync.SyncEngineWithGame();
        }
        catch (Exception e)
        {
            SyLog.Err(ELogTag.ProxyGame, e.ToString());
        }
    }

    public void EG_Run(TimeData timeData)
    {
        try
        {
            _ecs.GetSingleton<TimeData>() = timeData;
            _ecs.RunSystems();
            _ecsSync.SyncEngineWithGame();
        }
        catch (Exception e)
        {
            SyLog.Err(ELogTag.ProxyGame, e.ToString());
        }
    }

    public void EG_Destroy()
    {
        try
        {
            _ecs.DestroySystems();
        }
        catch (Exception e)
        {
            SyLog.Err(ELogTag.ProxyGame, e.ToString());
        }
    }


    //-----------------------------------------------------------
    //-----------------------------------------------------------
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern uint GE_CreateEngineEntity();
    
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void GE_DestroyEngineEntity(uint engineEnt);
    
    //-----------------------------------------------------------
    //-----------------------------------------------------------
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern uint GE_AddTransformComp(uint engineEnt);
    
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern uint GE_RemoveTransformComp(uint engineEnt);
    
    
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void GE_UpdateTransformComp(uint engineEnt, TransformComp comp);
    
    public void EG_UpdateTransformComp(uint engineEnt, TransformComp comp)
    {
        try
        {
            _ecsSync.ReceiveTransformFromEngine(engineEnt, comp);
        }
        catch (Exception e)
        {
            SyLog.Err(ELogTag.ProxyGame, e.ToString());
        }
    }

    //-----------------------------------------------------------
    //-----------------------------------------------------------
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void GE_AddMeshComp(uint engineEnt);
    
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void GE_RemoveMeshComp(uint engineEnt);
    
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void GE_UpdateMeshComp(uint engineEnt, MeshComp comp);

    public void EG_UpdateMeshComp(uint engineEnt, MeshComp comp)
    {
        try
        {
            //_ecsSync.ReceiveMeshFromEngine(engineEnt, comp);
        }
        catch (Exception e)
        {
            SyLog.Err(ELogTag.ProxyGame, e.ToString());
        }
    }
}
}