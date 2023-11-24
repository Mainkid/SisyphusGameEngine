using Leopotam.EcsLite;
using SyEngine.Core;

namespace SyEngine.Editor.Drawers
{
public interface IEditorDrawerComp
{
	void DrawComp(int ent);
}

public class EditorDrawerComp<T> : EditorDrawerBase<T>, IEditorDrawerComp where T: struct
{
	private readonly EcsPool<T> _pool;

	private EditorDrawerReflect<T> _fallbackDrawer;

	public EditorDrawerComp(SyProxyEditor editor, SyEcs ecs) : base(editor)
	{
		_pool = ecs.World.GetPool<T>();
	}

	public void DrawComp(int ent)
	{
		SyProxyEditor.GeDrawSeparator(typeof(T).Name);

		ref var comp = ref _pool.Get(ent);
		Draw(null, ref comp);
		_pool.Get(ent) = comp;
	}
	
	public override bool DrawRaw(string name, ref object rawVal)
	{
		if (_fallbackDrawer == null)
			_fallbackDrawer = new EditorDrawerReflect<T>(Editor);
		return _fallbackDrawer.DrawRaw(name, ref rawVal);
	}

	public override bool Draw(string name, ref T val)
	{
		if (_fallbackDrawer == null)
			_fallbackDrawer = new EditorDrawerReflect<T>(Editor);
		return _fallbackDrawer.Draw(name, ref val);
	}

}
}