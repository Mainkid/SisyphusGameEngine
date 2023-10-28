using Leopotam.EcsLite;
using SyEngine.Game;
using SyEngine.Game.Comps;
using SyEngine.Game.Datas;
using SyEngine.Logger;

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
		Ecs.AddTransformComp(ent).Position = new SyVector3(-5, 0, 10);
		Ecs.AddMeshComp(ent);
		
		SyLog.Debug(ELogTag.Test, $"ent g{ent} initial tf {Ecs.GetComp<TransformComp>(ent)}");
	}

	public override void Run()
	{
		float time      = Ecs.GetSingleton<TimeData>().TotalTime;
		float deltaTime = Ecs.GetSingleton<TimeData>().DeltaTime;
		
		if (time > _nextLogTime)
		{
			_nextLogTime = time + 1f;
			foreach (int ent in _filter)
			{
				ref var tf = ref Ecs.GetComp<TransformComp>(ent);
				tf.Position.X += 1;
				SyLog.Debug(ELogTag.Test, $"ent g{ent} tf {tf}");

				if (tf.Position.X >= 5f)
				{
					SyLog.Debug(ELogTag.Test, $"ent g{ent} destroy");
					Ecs.DestroyEntity(ent);
				}
			}
		}
	}

	public override void Destroy() { }
}
}