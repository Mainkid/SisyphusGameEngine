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
	[MethodImpl(MethodImplOptions.InternalCall)]
	public static extern uint GeCreateEngineEntity();

	[MethodImpl(MethodImplOptions.InternalCall)]
	public static extern void GeDestroyEngineEntity(uint engineEnt);

	public void EgContinueEntityDestroyCascade(uint engineEnt)
	{
		try
		{
			Ecs.ContinueEntityDestroyCascade(engineEnt);
		}
		catch (Exception e)
		{
			SyLog.Err(ELogTag.Ecs, e.ToString());
		}
	}

	//-----------------------------------------------------------
	//-----------------------------------------------------------
	[MethodImpl(MethodImplOptions.InternalCall)]
	public static extern uint GeAddTransformComp(uint engineEnt);

	[MethodImpl(MethodImplOptions.InternalCall)]
	public static extern uint GeRemoveTransformComp(uint engineEnt);


	[MethodImpl(MethodImplOptions.InternalCall)]
	public static extern void GeUpdateTransformComp(uint engineEnt, ProxyTransformComp proxy);

	public void EgUpdateTransformComp(uint engineEnt, ProxyTransformComp proxy)
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
	public static extern void GeAddMeshComp(uint engineEnt);

	[MethodImpl(MethodImplOptions.InternalCall)]
	public static extern void GeRemoveMeshComp(uint engineEnt);

	[MethodImpl(MethodImplOptions.InternalCall)]
	public static extern void GeUpdateMeshComp(uint engineEnt, MeshComp comp);

	public void EgUpdateMeshComp(uint engineEnt, MeshComp comp)
	{
		try
		{
			//_ecsSync.ReceiveMeshFromEngine(engineEnt, comp);
		}
		catch (Exception e)
		{
			SyLog.Err(ELogTag.ProxyEcs, e.ToString());
		}
	}
}
}