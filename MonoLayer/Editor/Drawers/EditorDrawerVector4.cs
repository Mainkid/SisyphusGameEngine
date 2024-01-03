using SyEngine.Datas;
using SyEngine.Ecs;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerVector4 : EditorDrawerBase<SyVector4>
{
	public EditorDrawerVector4(SyProxyEditor editor, SyEcs ecs) : base(editor, ecs) { }
	
	public override bool Draw(string name, ref SyVector4 val)
		=> SyProxyEditor.GeDrawVector4Field(name, ref val);
}
}