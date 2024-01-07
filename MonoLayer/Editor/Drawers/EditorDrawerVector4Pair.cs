using SyEngine.Datas;
using SyEngine.Ecs;

namespace SyEngine.Editor.Drawers
{
public class EditorDrawerVector4Pair : EditorDrawerBase<SyVector4Pair>
{
	public EditorDrawerVector4Pair(SyProxyEditor editor, SyEcs ecs) : base(editor, ecs) { }
	
	public override bool Draw(string name, ref SyVector4Pair val)
	{
		var isChanged = false;
		isChanged |= SyProxyEditor.GeDrawVector4Field($"{name}.A", ref val.A);
		isChanged |= SyProxyEditor.GeDrawVector4Field($"{name}.B", ref val.B);
		return isChanged;
	}
}
}