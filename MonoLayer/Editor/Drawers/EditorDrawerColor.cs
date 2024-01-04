using SyEngine.Datas;
using SyEngine.Ecs;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerColor : EditorDrawerBase<SyColor>
{
	public EditorDrawerColor(SyProxyEditor editor, SyEcs ecs) : base(editor, ecs) { }
	
	public override bool Draw(string name, ref SyColor val)
		=> SyProxyEditor.GeDrawColorField(name, true, ref val);
}
}