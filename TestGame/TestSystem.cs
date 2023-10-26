using System;
using Leopotam.EcsLite;
using SyEngine.Game;
using SyEngine.Game.Comps;

namespace TestGame
{
public class TestSystem : SyEcsSystemBase
{
	private EcsFilter _filter;

	private float _nextLogTime;

	//-----------------------------------------------------------
	//-----------------------------------------------------------
	public override void Init()
	{
		_filter = Ecs.BuildFilter<TransformComp>().End();

		int ent = Ecs.CreateEntity();
		Ecs.AddTransformComp(ent);
		Ecs.AddMeshComp(ent);
	}

	public override void Run()
	{
		float time      = Ecs.GetSingleton<TimeData>().TotalTime;
		float deltaTime = Ecs.GetSingleton<TimeData>().DeltaTime;
		
		if (time > _nextLogTime)
		{
			Console.WriteLine($"[game] TestSystem. time: {time}, deltaTime: {deltaTime}");
			_nextLogTime = time + 1f;
		}
		
		//foreach (int ent in _filter)
		//	Ecs.GetComp<TransformComp>(ent).Position
	}

	public override void Destroy() { }
}
}