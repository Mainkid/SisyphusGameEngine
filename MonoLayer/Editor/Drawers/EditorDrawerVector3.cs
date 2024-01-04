using SyEngine.Datas;
using SyEngine.Ecs;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerVector3 : EditorDrawerBase<SyVector3>
{
	public EditorDrawerVector3(SyProxyEditor editor, SyEcs ecs) : base(editor, ecs) { }
	
	public override bool Draw(string name, ref SyVector3 val)
		=> SyProxyEditor.GeDrawVector3Field(name, ref val);
}
}