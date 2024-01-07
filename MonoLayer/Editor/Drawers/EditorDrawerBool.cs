using SyEngine.Ecs;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerBool : EditorDrawerBase<bool>
{
	public EditorDrawerBool(SyProxyEditor editor, SyEcs ecs) : base(editor, ecs) { }
	
	public override bool Draw(string name, ref bool val)
		=> SyProxyEditor.GeDrawBoolField(name, ref val);
}
}