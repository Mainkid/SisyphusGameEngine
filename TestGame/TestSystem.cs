using Leopotam.EcsLite;
using SyEngine.Core;
using SyEngine.Core.Comps;
using SyEngine.Core.Datas;
using SyEngine.Logger;

namespace TestGame
{
struct ParentTag : SyEcs.IComp { }
struct ChildTag : SyEcs.IComp { }

struct TestEditorComp : SyEcs.IComp
{
	public int   TestIntVal;
	public bool  TestBoolVal;
	public float TestFloatVal;
}

public class TestSystem : SyEcsSystemBase
{
	private EcsFilter _filter;

	private float _nextLogTime;

	//-----------------------------------------------------------
	//-----------------------------------------------------------
	public override void Init()
	{
		_filter = Ecs.BuildFilter<ParentTag>().Inc<TransformComp>().End();

		int parentEnt = Ecs.CreateEntity();
		Ecs.AddTransformComp(parentEnt).LocalPosition = new SyVector3(-5, 0, 10);
		Ecs.AddMeshComp(parentEnt);
		Ecs.AddComp<ParentTag>(parentEnt);
		ref var testEditorComp = ref Ecs.AddComp<TestEditorComp>(parentEnt);
		testEditorComp.TestIntVal   = 1;
		testEditorComp.TestFloatVal = 1.2f;
		testEditorComp.TestBoolVal  = true;

		int     childEnt = Ecs.CreateEntity();
		ref var childTf  = ref Ecs.AddTransformComp(childEnt);
		childTf.LocalPosition = new SyVector3(-5, 10, 10);
		childTf.ParentEnt     = parentEnt;
		Ecs.AddMeshComp(childEnt);
		Ecs.AddComp<ChildTag>(childEnt);
	}

	public override void Run()
	{
		return;
		float time      = Ecs.GetSingleton<TimeData>().TotalTime;
		float deltaTime = Ecs.GetSingleton<TimeData>().DeltaTime;
		
		if (time > _nextLogTime)
		{
			_nextLogTime = time + 1f;
			foreach (int ent in _filter)
			{
				ref var tf = ref Ecs.GetComp<TransformComp>(ent);
				tf.LocalPosition.X += 1;
				SyLog.Debug(ELogTag.Test, $"ent g{ent} tf {tf}");

				if (tf.Position.X >= 20f)
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