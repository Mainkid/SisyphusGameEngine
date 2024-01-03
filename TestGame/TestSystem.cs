using LeoEcs;
using SyEngine.Datas;
using SyEngine.Ecs;
using SyEngine.Ecs.Comps;
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
		return;
		_filter = Ecs.BuildFilter<ParentTag>().Inc<TransformComp>().End();

		int entA = Ecs.CreateEntity(true);
		Ecs.GetComp<TransformComp>(entA).Position = new SyVector3(-10, 0, 10);
		Ecs.AddComp<MeshComp>(entA);
		Ecs.AddComp<ParentTag>(entA);
		ref var entATest = ref Ecs.AddComp<TestEditorComp>(entA);
		entATest.TestIntVal   = 1;
		entATest.TestFloatVal = 1.2f;
		entATest.TestBoolVal  = true;

		int     entB = Ecs.CreateEntity(true);
		ref var entBTf  = ref Ecs.GetComp<TransformComp>(entB);
		entBTf.Position = new SyVector3(0, 10, 0);
		entBTf.ParentEnt     = entA;
		Ecs.AddComp<MeshComp>(entB);
		Ecs.AddComp<ChildTag>(entB);

		int entC = Ecs.CreateEntity(true);
		Ecs.GetComp<TransformComp>(entC).Position = new SyVector3(10, 10, 10);
		Ecs.AddComp<LightComp>(entC);

		int entD = Ecs.CreateEntity(true);
		Ecs.GetComp<TransformComp>(entD).Position = new SyVector3(0, 5, 20);
		Ecs.AddComp<MeshComp>(entD);
		ref var entDRigid    = ref Ecs.AddComp<RigidComp>(entD);
		ref var entDCollider = ref Ecs.AddComp<ColliderComp>(entD);
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