using SyEngine.Ecs;
using SyEngine.Ecs.Comps;
using LeoEcs;

namespace SyEngine.Editor.Drawers
{
public interface IEditorDrawerComp
{
	void DrawComp(int ent);
}

public class EditorDrawerComp<T> : EditorDrawerBase<T>, IEditorDrawerComp where T: struct
{
	private readonly EcsPool<T> _pool;
	private readonly bool       _isRemovable;

	private EditorDrawerReflect<T> _fallbackDrawer;

	public EditorDrawerComp(SyProxyEditor editor, SyEcs ecs) : base(editor, ecs)
	{
		_pool = ecs.World.GetPool<T>();
		_isRemovable = typeof(T) != typeof(SceneObjectComp) &&
		               typeof(T) != typeof(TransformComp);
	}

	public void DrawComp(int ent)
	{
		string name = typeof(T).Name;
		
		SyProxyEditor.GeBeginGroup(name);
		
		int action = SyProxyEditor.GeBeginComp(typeof(T).Name, _isRemovable);
		if (action == 0)
		{
			Ecs.RemoveCompRaw(typeof(T), ent);
		}
		else
		{
			ref var comp = ref _pool.Get(ent);
			Draw(null, ref comp);
			_pool.Get(ent) = comp;	
		}
		SyProxyEditor.GeEndComp();
		
		SyProxyEditor.GeEndGroup();
	}

	public override bool Draw(string name, ref T val)
	{
		if (_fallbackDrawer == null)
			_fallbackDrawer = new EditorDrawerReflect<T>(Editor, Ecs);
		return _fallbackDrawer.Draw(name, ref val);
	}
}
}