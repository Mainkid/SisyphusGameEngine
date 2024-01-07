using SyEngine.Ecs;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerInt : EditorDrawerBase<int>
{
	public EditorDrawerInt(SyProxyEditor editor, SyEcs ecs) : base(editor, ecs) { }
	
	public override bool Draw(string name, ref int val)
		=> SyProxyEditor.GeDrawIntField(name, ref val);
}
}