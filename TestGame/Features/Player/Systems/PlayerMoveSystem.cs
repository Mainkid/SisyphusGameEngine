using System;
using LeoEcs;
using SyEngine.Datas;
using SyEngine.Ecs;
using SyEngine.Ecs.Comps;
using SyEngine.Input;
using TestGame.Features.Player.Comps;

namespace TestGame
{
public class PlayerMoveSystem : SyEcsSystemBase
{
	private EcsFilter _playersFilter;
	
	public override void Init()
	{
		_playersFilter = Ecs.BuildFilter<PlayerData>().Inc<TransformComp>().End();
	}

	public override void Run()
	{
		foreach (int ent in _playersFilter)
		{
			if (Input.IsDown(EKey.W))
				Move(ent, new SyVector3(0, 0, 1.0f));
			if (Input.IsDown(EKey.S))
				Move(ent, new SyVector3(0, 0, -1.0f));
			if (Input.IsDown(EKey.A))
				Move(ent, new SyVector3(-1.0f, 0, 0));
			if (Input.IsDown(EKey.D))
				Move(ent, new SyVector3(1.0f, 0, 0));
		}
	}

	public override void Destroy() { }


	private void Move(int ent, SyVector3 dir)
	{
		ref var player = ref Ecs.GetComp<PlayerData>(ent);
		ref var tf     = ref Ecs.GetComp<TransformComp>(ent);

		tf.Position += dir * player.MoveSpeed * Ecs.GetSingleton<TimeData>().DeltaTime;
	}
}
}