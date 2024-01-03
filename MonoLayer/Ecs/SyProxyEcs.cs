using System;
using System.Runtime.CompilerServices;
using SyEngine.Ecs.Sync;
using SyEngine.Logger;

namespace SyEngine.Ecs
{
internal class SyProxyEcs
{
	public readonly SyEcs Ecs;

	public readonly SyEcsSyncer Syncer;

	public SyProxyEcs()
	{
		Ecs  = new SyEcs();
		Syncer = new SyEcsSyncer(Ecs);
	}

	//-----------------------------------------------------------
	//-----------------------------------------------------------
	private void EgSyncEngineWithGame()
	{
		try
		{
			Syncer.TrySendAll();
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
	public static extern void GeUpdateSceneObjectComp(uint engineEnt, ProxySceneObjectComp proxy);

	private void EgUpdateSceneObjectComp(uint engineEnt, ProxySceneObjectComp proxy)
	{
		try
		{
			Syncer.Receive(engineEnt, ref proxy);
		}
		catch (Exception e)
		{
			SyLog.Err(ELogTag.ProxyEcs, e.ToString());
		}
	}
	
	//-----------------------------------------------------------
	//-----------------------------------------------------------
	[MethodImpl(MethodImplOptions.InternalCall)]
	public static extern void GeUpdateTransformComp(uint engineEnt, ProxyTransformComp proxy);

	private void EgUpdateTransformComp(uint engineEnt, ProxyTransformComp proxy)
	{
		try
		{
			Syncer.Receive(engineEnt, ref proxy);
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
			Syncer.Receive(engineEnt, ref proxy);
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

	private void EgUpdateLightComp(uint engineEnt, ProxyLightComp proxy)
	{
		try
		{
			Syncer.Receive(engineEnt, ref proxy);
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
			Syncer.Receive(engineEnt, ref proxy);
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
			Syncer.Receive(engineEnt, ref proxy);
		}
		catch (Exception e)
		{
			SyLog.Err(ELogTag.ProxyEcs, e.ToString());
		}
	}
	
	//-----------------------------------------------------------
	//-----------------------------------------------------------
	[MethodImpl(MethodImplOptions.InternalCall)]
	public static extern void GeUpdateSkyboxComp(uint engineEnt, ProxySkyboxComp proxy);

	private void EgUpdateSkyboxComp(uint engineEnt, ProxySkyboxComp proxy)
	{
		try
		{
			Syncer.Receive(engineEnt, ref proxy);
		}
		catch (Exception e)
		{
			SyLog.Err(ELogTag.ProxyEcs, e.ToString());
		}
	}

	//-----------------------------------------------------------
	//-----------------------------------------------------------
	[MethodImpl(MethodImplOptions.InternalCall)]
	public static extern void GeUpdateParticlesComp(uint engineEnt, ProxyParticlesComp proxy);

	private void EgUpdateParticlesComp(uint engineEnt, ProxyParticlesComp proxy)
	{
		try
		{
			Syncer.Receive(engineEnt, ref proxy);
		}
		catch (Exception e)
		{
			SyLog.Err(ELogTag.ProxyEcs, e.ToString());
		}
	}

	//-----------------------------------------------------------
	//-----------------------------------------------------------
	[MethodImpl(MethodImplOptions.InternalCall)]
	public static extern void GeUpdateSoundComp(uint engineEnt, ProxySoundComp proxy);

	private void EgUpdateSoundComp(uint engineEnt, ProxySoundComp proxy)
	{
		try
		{
			Syncer.Receive(engineEnt, ref proxy);
		}
		catch (Exception e)
		{
			SyLog.Err(ELogTag.ProxyEcs, e.ToString());
		}
	}

	//-----------------------------------------------------------
	//-----------------------------------------------------------
	[MethodImpl(MethodImplOptions.InternalCall)]
	public static extern void GeUpdateFixedJointComp(uint engineEnt, ProxyFixedJointComp proxy);

	private void EgUpdateFixedJointComp(uint engineEnt, ProxyFixedJointComp proxy)
	{
		try
		{
			Syncer.Receive(engineEnt, ref proxy);
		}
		catch (Exception e)
		{
			SyLog.Err(ELogTag.ProxyEcs, e.ToString());
		}
	}
	
	//-----------------------------------------------------------
	//-----------------------------------------------------------
	[MethodImpl(MethodImplOptions.InternalCall)]
	public static extern void GeUpdateHingeJointComp(uint engineEnt, ProxyHingeJointComp proxy);

	private void EgUpdateHingeJointComp(uint engineEnt, ProxyHingeJointComp proxy)
	{
		try
		{
			Syncer.Receive(engineEnt, ref proxy);
		}
		catch (Exception e)
		{
			SyLog.Err(ELogTag.ProxyEcs, e.ToString());
		}
	}
}
}