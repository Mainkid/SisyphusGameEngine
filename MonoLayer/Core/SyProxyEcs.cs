using System;
using System.Runtime.CompilerServices;
using SyEngine.Core.Comps;
using SyEngine.Core.ProxyComps;
using SyEngine.Logger;

namespace SyEngine.Core
{
internal class SyProxyEcs
{
	public readonly SyEcs Ecs;

	public readonly SyEcsSync Sync;

	public SyProxyEcs()
	{
		Ecs  = new SyEcs();
		Sync = new SyEcsSync(Ecs);
	}

	//-----------------------------------------------------------
	//-----------------------------------------------------------
	private void EgSyncEngineWithGame()
	{
		try
		{
			Sync.SyncEngineWithGame();
		}
		catch (Exception e)
		{
			SyLog.Err(ELogTag.ProxyEcs, e.ToString());
		}
	}

	//-----------------------------------------------------------
	//-----------------------------------------------------------
	[MethodImpl(MethodImplOptions.InternalCall)]
	public static extern uint GeCreateEngineEntity();
	
	
	[MethodImpl(MethodImplOptions.InternalCall)]
	public static extern void GeDestroyEngineEntity(uint engineEnt);

	private void EgDestroyEntity(uint engineEnt)
	{
		try
		{
			Ecs.DestroyGameEntityByEngine(engineEnt);
		}
		catch (Exception e)
		{
			SyLog.Err(ELogTag.ProxyEcs, e.ToString());
		}
	}

	//-----------------------------------------------------------
	//-----------------------------------------------------------
	[MethodImpl(MethodImplOptions.InternalCall)]
	public static extern void GeAddEngineComp(uint engineEnt, EEngineCompId id);

	[MethodImpl(MethodImplOptions.InternalCall)]
	public static extern void GeRemoveEngineComp(uint engineEnt, EEngineCompId id);

	//-----------------------------------------------------------
	//-----------------------------------------------------------
	[MethodImpl(MethodImplOptions.InternalCall)]
	public static extern void GeUpdateTransformComp(uint engineEnt, ProxyTransformComp proxy);

	private void EgUpdateTransformComp(uint engineEnt, ProxyTransformComp proxy)
	{
		try
		{
			Sync.ReceiveTransformFromEngine(engineEnt, proxy);
		}
		catch (Exception e)
		{
			SyLog.Err(ELogTag.ProxyEcs, e.ToString());
		}
	}

	//-----------------------------------------------------------
	//-----------------------------------------------------------
	[MethodImpl(MethodImplOptions.InternalCall)]
	public static extern void GeUpdateMeshComp(uint engineEnt, ProxyMeshComp proxy);

	private void EgUpdateMeshComp(uint engineEnt, ProxyMeshComp proxy)
	{
		try
		{
			Sync.ReceiveMeshFromEngine(engineEnt, proxy);
		}
		catch (Exception e)
		{
			SyLog.Err(ELogTag.ProxyEcs, e.ToString());
		}
	}

	//-----------------------------------------------------------
	//-----------------------------------------------------------
	[MethodImpl(MethodImplOptions.InternalCall)]
	public static extern void GeUpdateLightComp(uint engineEnt, ProxyLightComp proxy);

	private void EgUpdateLightComp(uint engineEnt, ProxyLightComp comp)
	{
		try
		{
			Sync.ReceiveLightFromEngine(engineEnt, comp);
		}
		catch (Exception e)
		{
			SyLog.Err(ELogTag.ProxyEcs, e.ToString());
		}
	}
	//-----------------------------------------------------------
	//-----------------------------------------------------------
	[MethodImpl(MethodImplOptions.InternalCall)]
	public static extern void GeUpdateColliderComp(uint engineEnt, ProxyColliderComp proxy);

	private void EgUpdateColliderComp(uint engineEnt, ProxyColliderComp proxy)
	{
		try
		{
			Sync.ReceiveColliderFromEngine(engineEnt, proxy);
		}
		catch (Exception e)
		{
			SyLog.Err(ELogTag.ProxyEcs, e.ToString());
		}
	}
	
	//-----------------------------------------------------------
	//-----------------------------------------------------------
	[MethodImpl(MethodImplOptions.InternalCall)]
	public static extern void GeUpdateRigidComp(uint engineEnt, ProxyRigidComp proxy);

	private void EgUpdateRigidComp(uint engineEnt, ProxyRigidComp proxy)
	{
		try
		{
			Sync.ReceiveRigidFromEngine(engineEnt, proxy);
		}
		catch (Exception e)
		{
			SyLog.Err(ELogTag.ProxyEcs, e.ToString());
		}
	}
	
	//-----------------------------------------------------------
	//-----------------------------------------------------------
}
}