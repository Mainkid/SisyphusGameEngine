using System;
using LeoEcs;

namespace SyEngine.Ecs.Sync
{
internal abstract class SyEcsSyncBase<TComp, TProxy> : ISyEcsSync<TProxy> where TComp: struct
{
	protected readonly SyEcs          Ecs;
	protected readonly EcsPool<TComp> Pool;
	protected readonly EcsFilter      Filter;

	public SyEcsSyncBase(SyEcs ecs)
	{
		Ecs    = ecs;
		Pool   = Ecs.World.GetPool<TComp>();
		Filter = Ecs.World.Filter<TComp>().End();
	}
	//-----------------------------------------------------------
	//-----------------------------------------------------------
	public abstract EEngineCompId Id { get; }

	public void TrySendAll()
	{
		foreach (int ent in Filter)
		{
			ref var comp = ref Pool.Get(ent);
			int     hash = comp.GetHashCode();
			if (hash == GetHashImpl(ref comp))
				continue;
			SetHashImpl(ref comp, hash);
			SendImpl(Ecs.ToEngineEnt(ent), ref comp);
			
			Console.WriteLine($"[TEST] g{ent} {Id} sent to engine");
		}
	}

	public void Receive(uint engineEnt, ref TProxy proxy)
	{
		int gameEnt = Ecs.GetOrCreateEntitiesPairByEngineEnt(engineEnt);
		if (Pool.Has(gameEnt))
		{
			ref var comp = ref Pool.Get(gameEnt);
			ReceiveImpl(ref proxy, ref comp);
            SetHashImpl(ref comp, comp.GetHashCode());
		}
		else
		{
			ref var comp = ref Pool.Add(gameEnt);
			ReceiveImpl(ref proxy, ref comp);
			SetHashImpl(ref comp, comp.GetHashCode());
		}
		
		Console.WriteLine($"[TEST] g{gameEnt} {Id} received from engine");
	}


	public Type ProxyType => typeof(TProxy);
	

	protected abstract void SendImpl(uint          engineEnt, ref TComp comp);
	protected abstract void ReceiveImpl(ref TProxy proxy,     ref TComp comp);

	protected abstract int? GetHashImpl(ref TComp comp);
	protected abstract void SetHashImpl(ref TComp comp, int hash);
}
}