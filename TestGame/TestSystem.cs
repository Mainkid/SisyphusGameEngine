using Leopotam.EcsLite;
using SyEngine.Core;
using SyEngine.Core.Comps;
using SyEngine.Core.Datas;
using SyEngine.Logger;

namespace TestGame
{
struct ParentTag : SyEcs.IGameComp { }
struct ChildTag : SyEcs.IGameComp { }

struct TestEditorComp : SyEcs.IGameComp
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

		int entA = Ecs.CreateEntity();
		Ecs.AddTransformComp(entA).Position = new SyVector3(-10, 0, 10);
		Ecs.AddMeshComp(entA);
		Ecs.AddComp<ParentTag>(entA);
		ref var entATest = ref Ecs.AddComp<TestEditorComp>(entA);
		entATest.TestIntVal   = 1;
		entATest.TestFloatVal = 1.2f;
		entATest.TestBoolVal  = true;

		int     entB = Ecs.CreateEntity();
		ref var entBTf  = ref Ecs.AddTransformComp(entB);
		entBTf.Position = new SyVector3(0, 10, 0);
		entBTf.ParentEnt     = entA;
		Ecs.AddMeshComp(entB);
		Ecs.AddComp<ChildTag>(entB);

		int entC = Ecs.CreateEntity();
		Ecs.AddTransformComp(entC).Position = new SyVector3(10, 10, 10);
		Ecs.AddLightComp(entC);

		int entD = Ecs.CreateEntity();
		Ecs.AddTransformComp(entD).Position = new SyVector3(0, 5, 20);
		Ecs.AddMeshComp(entD);
		ref var entDRigid    = ref Ecs.AddRigidComp(entD);
		ref var entDCollider = ref Ecs.AddColliderComp(entD);
		entDCollider.Extent = SyVector3.One;
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