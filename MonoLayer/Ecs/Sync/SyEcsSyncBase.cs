using System;
using LeoEcs;

namespace SyEngine.Ecs.Sync
{
internal abstract class SyEcsSyncBase<TComp, TProxy> : ISyEcsSync<TProxy> where TComp: struct
{
	protected readonly SyEcs          _ecs;
	protected readonly EcsPool<TComp> _pool;
	protected readonly EcsFilter      _filter;

	public SyEcsSyncBase(SyEcs ecs)
	{
		_ecs    = ecs;
		_pool   = _ecs.World.GetPool<TComp>();
		_filter = _ecs.World.Filter<TComp>().End();
	}
	//-----------------------------------------------------------
	//-----------------------------------------------------------
	public abstract EEngineCompId Id { get; }

	public void TrySendAll()
	{
		foreach (int ent in _filter)
		{
			ref var comp = ref _pool.Get(ent);
			int     hash = comp.GetHashCode();
			if (hash == GetHashImpl(ref comp))
				continue;
			SetHashImpl(ref comp, hash);
			SendImpl(_ecs.ToEngineEnt(ent), ref comp);
			
			Console.WriteLine($"[TEST] g{ent} {Id} sent to engine");
		}
	}

	public void Receive(uint engineEnt, ref TProxy proxy)
	{
		int gameEnt = _ecs.GetOrCreateEntitiesPairByEngineEnt(engineEnt);
		if (_pool.Has(gameEnt))
		{
			ref var comp = ref _pool.Get(gameEnt);
			ReceiveImpl(ref proxy, ref comp);
            SetHashImpl(ref comp, comp.GetHashCode());
		}
		else
		{
			ref var comp = ref _pool.Add(gameEnt);
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