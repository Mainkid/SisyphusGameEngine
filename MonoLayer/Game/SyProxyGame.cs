using System;
using System.Runtime.CompilerServices;
using SyEngine.Game.Comps;

namespace SyEngine.Game
{
public class SyProxyGame
{
    private readonly SyGameInternalContext _context;

    private readonly SyEcs     _ecs;
    private readonly SyEcsSync _ecsSync;

    public SyProxyGame()
    {
        try
        {
            _context = new SyGameInternalContext();

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
        Console.WriteLine("[game] receive EG_SetConfig");
        try
        {
            _ecs.SetSystems(config.GetSystems());
        }
        catch (Exception e)
        {
            Console.WriteLine(e);
        }
    }

    public void EG_Init()
    {
        try
        {
            _ecs.InitSystems();
        }
        catch (Exception e)
        {
            Console.WriteLine(e);
        }
    }

    public void EG_Run(TimeData timeData)
    {
        try
        {
            _ecs.GetSingleton<TimeData>() = timeData;
            
            _ecs.RunSystems();

            //_ecsSync.SendTransformsToEngine();
            //_ecsSync.SendMeshesToEngine();
        }
        catch (Exception e)
        {
            Console.WriteLine(e);
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
            Console.WriteLine(e);
        }
    }


    //-----------------------------------------------------------
    //-----------------------------------------------------------
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern uint GE_CreateEngineEntity();
    
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern uint GE_DestroyEngineEntity();
    
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
            //_ecsSync.ReceiveTransformFromEngine(engineEnt, comp);
        }
        catch (Exception e)
        {
            Console.WriteLine(e);
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
            Console.WriteLine(e);
        }
    }
}
}