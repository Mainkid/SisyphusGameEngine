﻿using System;
using SyEngine.Core;
using SyEngine.Core.Comps;
using SyEngine.Logger;

namespace SyEngine.Game
{
public class SyProxyGame
{
    private SyProxyEcs _proxyEcs;

    private SyScene _scene;
    
    private bool _isGameSystemsInited;
    
    
    private void EgInit(SyProxyEcs proxyEcs, SyGameConfigBase config)
    {
        SyLog.Info(ELogTag.ProxyGame, "game init starts..");
        try
        {
            _proxyEcs = proxyEcs;
            _proxyEcs.Ecs.SetSystems(config.GetSystems());

            _proxyEcs.Ecs.AddSingletonRaw<TimeData>();

            _scene = new SyScene(_proxyEcs.Ecs);
            _scene.Load();
            
            
            SyLog.Info(ELogTag.ProxyGame, "game init done");
        }
        catch (Exception e)
        {
            SyLog.Err(ELogTag.ProxyGame, e.Message);
        }
    }

    private void EgLoopRun(TimeData timeData)
    {
        try
        {
            if (!_isGameSystemsInited)
            {
                SyLog.Info(ELogTag.ProxyGame, "game loop init starts..");
                _proxyEcs.Ecs.InitSystems();
                SyLog.Info(ELogTag.ProxyGame, "game loop init done");
                _isGameSystemsInited = true;
            }
            
            _proxyEcs.Ecs.GetSingleton<TimeData>() = timeData;
            _proxyEcs.Ecs.RunSystems();
        }
        catch (Exception e)
        {
            SyLog.Err(ELogTag.ProxyGame, e.ToString());
        }
    }

    private void EgLoopDestroy()
    {
        SyLog.Info(ELogTag.ProxyGame, "game loop destroy starts..");
        try
        {
            _proxyEcs.Ecs.DestroySystems();
            
            SyLog.Info(ELogTag.ProxyGame, "game loop destroy done");
        }
        catch (Exception e)
        {
            SyLog.Err(ELogTag.ProxyGame, e.ToString());
        }
    }

    
    private void EgSaveScene()
    {
        try
        {
            _scene.Save();
        }
        catch (Exception e)
        {
            SyLog.Err(ELogTag.ProxyGame, e.ToString());
        }
    }
    
    //-----------------------------------------------------------
    //-----------------------------------------------------------
}
}